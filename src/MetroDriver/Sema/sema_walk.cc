#include "AST.h"
#include "MetroDriver/sema.h"
#include "MetroDriver/evaluator.h"
#include "Error.h"
#include "Debug.h"
#include "Utils.h"

namespace metro {

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
    case ASTKind::Type:
    case ASTKind::Argument:
    case ASTKind::Value:
    case ASTKind::Variable:
    case ASTKind::Callfunc:
      ret = sema_factor(ast);
      break;

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

        if( !vartype.is_mutable ) {
          Error(ErrorKind::NotMutable, expr->token, "left side is not mutable").emit(true);
        }

        if( std::find(root->elements.begin(), root->elements.end(), var->defined) != root->elements.end() ) {
          if( !find_var_context(var->defined)->was_type_analyzed ) {
            Error(ErrorKind::UninitializedValue, var->token, "uninitialized value").emit(true);
          }
        }

        auto context = find_var_context(var->defined);

        alert;
        assert(context != nullptr);

        //context->type = vartype;

        auto rhs = walk(expr->rhs);

        if( context->was_type_analyzed ) {
          if( !vartype.equals(rhs) ) {
            Error(ErrorKind::TypeMismatch, expr->token, "assignment type mismatch").emit();
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

    //
    // variable definition
    case ASTKind::VarDefine: {
      auto let = (AST::VarDefine*)ast;
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

        // 参照型の場合は初期化式が必要
        if( context.type.is_reference ) {
          // 初期化式がない
          if( !let->init ) {
            Error(ErrorKind::TypeMismatch, let->token, "cannot define reference-type variable without initializer expression").emit(true);
          }
          // あるなら、左辺値じゃなければエラー
          else if( !get_type_attr(let->init).left ) {
            Error(ErrorKind::TypeMismatch, let->init, "expected lvalue expression").emit(true);
          }
        }
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

    case ASTKind::Return: {
      auto x = (AST::Return*)ast;

      if( this->cur_func_ast == nullptr )
        Error(ErrorKind::NotAllowed, x->token, "cannot use 'return' here").emit();

      this->walk(x->expr);

      break;
    }

    case ASTKind::Scope: {
      auto scope = (AST::Scope*)ast;

      if( scope->elements.empty() )
        break;

      auto& context = scopelist.emplace_front();

      context.scope = scope;

      for( auto&& i : scope->elements ) {
        context.cur_ast = i;

        walk(i);

        context.cur_index++;
      }

      // get final expressions
      auto lastexpr_list = get_final_expr_full(scope);

      for( auto begin = *lastexpr_list.begin(); auto&& last : lastexpr_list ) {
        // first
        if( last == begin ) {
          ret = walk(last);
        }
        else if( auto&& tmp = walk(last); !tmp.equals(ret) ) {
          Error(ErrorKind::TypeMismatch, last,
            Utils::linkstr("expected '", ret.to_string(),"' but found '",tmp.to_string(),"'"))
              .emit(true);
        }
      }

      scopelist.pop_front();

      break;
    }

    case ASTKind::Function: {
      auto func = (AST::Function*)ast;

      cur_func_ast = func;

      // arguments
      for( auto&& arg : func->args )
        walk(&arg);

      // return-type
      ret = analyze_func_return_type(func);

      // code
      walk(func->code);

      cur_func_ast = nullptr;

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

inline ValueType Sema::sema_factor(AST::Base* ast) {
  ValueType ret;

  switch( ast->kind ) {
    case ASTKind::None:
      break;

    case ASTKind::Type: {
      auto type = (AST::Type*)ast;

      alertios("type->name = " << type->name);

      for( auto&& pair : ValueType::name_table ) {
        if( type->name == pair.first ) {
          alert;
          ret = pair.second;
          goto _found_type;
        }
      }

      // TODO: find struct

      Error(ErrorKind::UndefinedTypeName, ast->token, "undefined type name").emit();

    _found_type:
      alert;

      ret.arr_depth = type->arr_depth;
      ret.have_elements = type->have_elements;

      ret.is_mutable = type->is_mutable;
      ret.is_reference = type->is_reference;

      for( auto&& sub : type->elems ) {
        ret.elems.emplace_back(walk(sub));
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
        Error(ErrorKind::UndefinedVariable, ast->token, "undefined variable name").emit(true);
      }
      else if( arrow_unini != var && var->defined->kind == ASTKind::VarDefine ) {
        if( auto ctx = find_var_context(var->defined); !ctx->was_type_analyzed ) {
          alertios(ctx);

          Error(ErrorKind::UninitializedValue, ast->token, "uninitialized value").emit(true);
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

    default:
      crash;
  }

  return ret;
}

} // namespace metro
