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
      case ASTKind::Type: {
        auto type = (AST::Type*)ast;

        alertios("type->name = " << type->name);

        if( type->name == "int" ) {
          alert;
          ret = ValueType::Kind::Int;
        }
        else {
          TODO_IMPL
        }

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
        }

        break;
      }

      case ASTKind::Callfunc: {
        auto call = (AST::CallFunc*)ast;

        // TODO: check argument types

        for( auto&& arg : call->args ) {
          walk(arg);
        }

        if( (call->callee = find_func(call->name)) == nullptr ) {
          if( (call->callee_builtin = find_builtin_func(call->name)) == nullptr ) {
            Error::add_error(ErrorKind::Undefined, ast->token, "undefined function name");
            Error::exit_app();
          }
        }
        else {
          ret = walk(call->callee);
        }

        alertios("callfunc " << call->name << ": " << ret.to_string());

        break;
      }

      // ------------------- //
      // ------------------- //
      case ASTKind::Function: {
        auto func = (AST::Function*)ast;

        cfn_ast = func;

        // arguments
        for(auto&&x:func->args){
          walk(&x);
        }

        // return-type
        analyze_func_return_type(ret, func);

        // code
        walk(func->code);

        cfn_ast = nullptr;
        
        break;
      }
      
      case ASTKind::If: {
        auto if_x = (AST::If*)ast;

        if(!walk(if_x->cond).equals(ValueType::Kind::Bool)){
          Error::add_error(ErrorKind::TypeMismatch, if_x->cond, "condision must be boolean");
        }

        ret = walk(if_x->if_true);

        if(if_x->if_false&&!ret.equals(walk(if_x->if_false))){
          Error::add_error(ErrorKind::TypeMismatch,if_x,"if-expr type mismatch");
        }

        break;
      }

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

      case ASTKind::Compare: {
        auto cmp = (AST::Compare*)ast;

        walk(cmp->first);

        for(auto&&item:cmp->list){
          walk(item.ast);
        }

        ret = ValueType::Kind::Bool;
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