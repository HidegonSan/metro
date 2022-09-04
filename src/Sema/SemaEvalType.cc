#include "AST.h"
#include "Error.h"
#include "Utils.h"
#include "MetroDriver/Sema.h"

namespace metro::semantics {

// --- //

/*
Sema::EvaluatedResult Sema::try_eval_type(AST::Base* ast) {
  using Cond = EvaluatedResult::Condition;

  switch( ast->kind ) {
    case ASTKind::Callfunc: {
      auto x = (AST::CallFunc*)ast;

      auto func = this->find_func(x->name);

      if( !func ) {
        Error(ErrorKind::UndefinedFunction,
          x->token,
          "undefined function name")
          .emit(true);
      }

      if( !func->dc.is_deducted )
        return Cond::Incomplete;

      for( auto&& arg : x->args ) {
        auto&& res = this->try_eval_type(arg);

        if( res.cond != Cond::Completed )
          return res;


      }

      break;
    }
  }

  return this->eval_type(ast);
}
*/

// ValueType Sema::eval_type(AST::Base* ast) {
Sema::EvalResult Sema::eval_type(AST::Base* ast) {
  using Cond = EvalResult::Condition;

  if( !ast )
    return { };

  if( this->caches.contains(ast) )
    return this->caches[ast];

  auto& result = this->caches[ast];

  auto& ret = result.type;

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

      Error(ErrorKind::UndefinedTypeName, ast->token, "undefined type name")
        .emit(true);

    _found_type:
      alert;

      ret.arr_depth = type->arr_depth;
      ret.have_elements = type->have_elements;

      ret.is_mutable = type->is_mutable;
      ret.is_reference = type->is_reference;

      for( auto&& sub : type->elems ) {
        auto&& res = this->eval_type(sub);

        ret.elems.emplace_back(res.type);
      }

      break;
    }

    case ASTKind::Argument: {
      auto argument = (AST::Argument*)ast;

      result = eval_type(argument->type);

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


      break;
    }

    case ASTKind::Callfunc: {
      auto x = (AST::CallFunc*)ast;

      auto func = this->find_func(x->name);

      if( !func ) {
        Error(ErrorKind::UndefinedFunction,
          x->token,
          "undefined function name")
          .emit(true);
      }

      if( !func->dc.is_deducted )
        return Cond::Incomplete;

      for( auto&& arg : x->args ) {
        auto&& res = this->eval_type(arg);

        if( res.cond != Cond::Completed )
          return res;


      }

      break;
    }

    case ASTKind::Compare: {
      auto cmp = (AST::Compare*)ast;

      eval_type(cmp->first);

      for( auto&& item : cmp->list ) {
        eval_type(item.ast);
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

      break;
    }

    case ASTKind::If: {
      auto if_x = (AST::If*)ast;

      if( !eval_type(if_x->cond).type.equals(ValueType::Kind::Bool) ) {
        Error(ErrorKind::TypeMismatch, if_x->cond, "condition must be boolean")
          .emit();
      }

      result = eval_type(if_x->if_true);
    
      if( if_x->if_false )
        if( auto&& tmp = this->eval_type(if_x->if_false); !ret.equals(tmp.type) )
          Error(ErrorKind::TypeMismatch, if_x->token, "if-expr type mismatch")
            .emit();

      break;
    }

    case ASTKind::For: {
      auto x = (AST::For*)ast;

      this->eval_type(x->init);

      auto&& cond = this->eval_type(x->cond);

      if( !cond.type.equals(ValueType::Kind::Bool) )
        Error(ErrorKind::TypeMismatch, x->cond, "condition must be boolean").emit();

      this->eval_type(x->counter);
      this->eval_type(x->code);

      break;
    }

    case ASTKind::Return: {
      auto x = (AST::Return*)ast;

      this->eval_type(x->expr);

      break;
    }

    case ASTKind::Scope: {
      auto scope = (AST::Scope*)ast;

      // 空のスコープ
      if( scope->elements.empty() )
        break;

      auto&& finals = this->get_returnable_expr(scope);

      for( auto&& elem : scope->elements )
        this->eval_type(elem);

      result = this->eval_type(*scope->elements.begin());

      for( auto it = scope->elements.begin() + 1; it != scope->elements.end(); it++ )
        if( auto&& tmp = this->eval_type(*it); !tmp.type.equals(result.type) )
          Error(ErrorKind::TypeMismatch, *it,
            Utils::linkstr(
              "expected '", result.type.to_string(),
              "' but found '", tmp.type.to_string(), "'"
            )
          )
           .emit();

      break;
    }

    case ASTKind::Function: {
      auto func = (AST::Function*)ast;

      if( !func->return_type ) {
        Error(ErrorKind::LanguageVersion, func->token,
          "function definition without return-type specification does not implemented yet")
          .add_help(func->code->token, "insert type name before this token")
          .emit(true);
      }

      // auto&& final_expr_list = get_returnable_expr(func->code);

      TODO_IMPL

      break;
    }

    default: {
      if( !ast->is_expr ) {
        alertfmt("%d", ast->kind);
        crash;
      }

      auto expr = (AST::Expr*)ast;

      auto lhs = eval_type(expr->lhs);
      auto rhs = eval_type(expr->rhs);

      // TODO: check operator

      ret = lhs.type;
      break;
    }
  }

  return ret;
}



} // namespace metro::semantics

