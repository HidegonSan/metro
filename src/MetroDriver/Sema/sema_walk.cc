#include "AST.h"
#include "MetroDriver/sema.h"
#include "MetroDriver/evaluator.h"
#include "Error.h"
#include "Debug.h"
#include "Utils.h"

namespace metro {

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
      auto x = (AST::Variable*)ast;

      auto var = get_var_context(x->name);

      if( !var ) {
        Error(ErrorKind::UndefinedVariable, ast->token, "undefined variable name").emit(true);
      }

      ret = var->type;
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
      auto x = (AST::Expr*)ast;

      TODO_IMPL

      break;
    }

    //
    // variable definition
    case ASTKind::VarDefine: {
      auto x = (AST::VarDefine*)ast;

      auto var_ctx = get_var_context(x->name, true);

      // 型の指定、初期化式　どっちもある
      if( x->type && x->init ) {
        var_ctx->type = walk(x->type);

        // 指定された型と初期化式の型が一致しない
        if( auto init_type = walk(x->init); !var_ctx->type.equals(init_type) ) {
          Error(ErrorKind::TypeMismatch, x->init, "expected '" + var_ctx->type.to_string() + "' but found '" + init_type.to_string() + "'")
            .add_help(x->type, "specified here")
            .emit();
        }
      }

      // 型の指定のみ
      else if( x->type ) {
        var_ctx->type = walk(x->type);
      }

      // 初期化式のみ
      else if( x->init ) {
        var_ctx->type = walk(x->init);
      }

      // どっちもない
      else {
        TODO_IMPL
      }

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

      // 空のスコープ
      if( scope->elements.empty() )
        break;

      auto& scope_ctx = scopelist.emplace_front();

      scope_ctx.ast = scope;

      for( auto&& i : scope->elements ) {
        scope_ctx.cur_ast = i;

        walk(i);

        scope_ctx.cur_index++;
      }

      // get final expressions
      auto&& lastexpr_list = get_results_wrap(scope);

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

      auto&& final_expr_list = get_results_wrap(func->code);

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
      if( !ast->is_expr ) {
        alertfmt("%d", ast->kind);
        crash;
      }

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

} // namespace metro
