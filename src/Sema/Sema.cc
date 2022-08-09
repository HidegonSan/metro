#include "AST.h"
#include "Sema/Sema.h"
#include "Error.h"
#include "Debug.h"
#include "Utils.h"
#include "MetroDriver/Evaluator.h"

namespace Metro::Semantics {

  // initialize Sema
  void Sema::init(AST::Scope* root) {
    this->root = root;

    // append all functions to functions
    for( auto scope = (AST::Scope*)root; auto&& x : scope->elems ) {
      if( x->kind == ASTKind::Function ) {
        functions.emplace_back((AST::Function*)x);
      }
    }
  }

  ValueType Sema::walk(AST::Base* ast) {
    if( !ast ) {
      return { };
    }

    if( walked.contains(ast) ) {
      return walked[ast];
    }

    auto& ret = walked[ast];

    switch( ast->kind ) {
      case ASTKind::None:
        break;

      case ASTKind::Type: {
        auto type = (AST::Type*)ast;

        alertios("type->name = " << type->name);

        for( auto&& pair : ValueType::name_table ) {
          if( type->name == pair.first ) {
            ret = pair.second;
            goto _found_type;
          }
        }

        // TODO: find struct

        Error::add_error(ErrorKind::Undefined, ast->token, "undefined type name");

      _found_type:
        break;
      }

      case ASTKind::Argument: {
        auto argument = (AST::Argument*)ast;

        ret = walk(argument->type);

        break;
      }

      case ASTKind::Value: {
        auto val = (AST::Value*)ast;

        val->object = create_obj(ast->token);

        ret = val->object->type;
        break;
      }

      case ASTKind::Variable: {
        auto var = (AST::Variable*)ast;

        if( (var->defined = find_var_defined(var->name)) == nullptr ) {
          Error::add_error(ErrorKind::Undefined, ast->token, "undefined variable name");
          Error::exit_app();
        }
        else if( arrow_unini != var && var->defined->kind == ASTKind::Let ) {
          if( auto ctx = find_var_context(var->defined); !ctx->was_type_analyzed ) {
            alertios(ctx);

            Error::add_error(ErrorKind::UninitializedValue, ast->token, "uninitialized value");
            Error::exit_app();
          }
        }

        ret = walk(var->defined);
        break;
      }

      case ASTKind::Callfunc: {
        ret = sema_callfunc((AST::CallFunc*)ast);

        alertios("callfunc " << ((AST::CallFunc*)ast)->name << ": " << ret.to_string());

        break;
      }

      case ASTKind::Compare: {
        auto cmp = (AST::Compare*)ast;

        walk(cmp->first);

        for( auto&& item : cmp->list ) {
          walk(item.ast);
        }

        ret = ValueType::Kind::Bool;
        break;
      }

      //
      // assignment
      case ASTKind::Assign: {
        auto expr = (AST::Expr*)ast;

        // TODO: get lhs_final (example: index-ref)

        // left is variable
        if( expr->lhs->kind == ASTKind::Variable ) {

          auto var = (AST::Variable*)expr->lhs;

          arrow_unini = var;
          auto&& vartype = walk(expr->lhs);

          if( std::find(root->elems.begin(), root->elems.end(), var->defined) != root->elems.end() ) {
            if( !find_var_context(var->defined)->was_type_analyzed ) {
              Error::add_error(ErrorKind::UninitializedValue, var->token, "uninitialized value");
              Error::exit_app();
            }
          }

          auto context = find_var_context(var->defined);

          alert;
          assert(context != nullptr);

          //context->type = vartype;

          auto rhs = walk(expr->rhs);

          if( context->was_type_analyzed ) {
            if( !vartype.equals(rhs) ) {
              Error::add_error(ErrorKind::TypeMismatch, expr->token, "assignment type mismatch");
            }
          }
          else {
            context->type = rhs;
            context->was_type_analyzed = true;
          }
        }

        arrow_unini = nullptr;
        break;
      }

      case ASTKind::Let: {
        auto let = (AST::Let*)ast;
        auto& scope = get_cur_scope();

        // shadowing
        if( scope.var_context.contains(let->name) ) {
          TODO_IMPL
        }

        auto& context = scope.var_context[let->name];

        context.defined = let;

        if( let->type != nullptr ) {
          context.was_type_analyzed = true;
          context.type = walk(let->type);
        }
        else if( let->init != nullptr ) {
          context.was_type_analyzed = true;
          context.type = walk(let->init);
        }

        ret = context.type;
        break;
      }

      case ASTKind::If:
      case ASTKind::For:
        ret = sema_controls(ast);
        break;

      case ASTKind::Scope: {
        auto scope = (AST::Scope*)ast;

        if( scope->elems.empty() ) {
          break;
        }

        auto& context = scopelist.emplace_front();

        context.scope = scope;

        auto iter = scope->elems.begin();
        auto last = scope->elems.end() - 1;

        for( ; iter != last; iter++ ) {
          context.cur_ast = *iter;
          walk(*iter);

          context.cur_index++;
        }

        context.cur_ast=*last;
        ret = walk(*last);

        scopelist.pop_front();

        break;
      }

      case ASTKind::Function: {
        auto func = (AST::Function*)ast;

        cfn_ast = func;

        // arguments
        for( auto&& arg : func->args ) {
          walk(&arg);
        }

        // return-type
        analyze_func_return_type(ret, func);

        // code
        walk(func->code);

        cfn_ast = nullptr;

        break;
      }

      default: {
        auto expr = (AST::Expr*)ast;

        auto lhs = walk(expr->lhs);
        auto rhs = walk(expr->rhs);

        // TODO: check operator

        ret = lhs;
        break;
      }
    }

    return ret;
  }
}