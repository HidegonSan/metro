#include "AST.h"
#include "Error.h"
#include "Utils.h"
#include "MetroDriver/Sema.h"

namespace metro::semantics {


Sema::EvaluatedResult Sema::try_eval_type(AST::Base* ast) {
  /*

  失敗する可能性があるもの:
    CallFunc    // 戻り値の型が不明

  */

  using Cond = EvaluatedResult::Condition;

  if( !ast )
    return Cond::NullPointer;

  switch( ast->kind ) {
    case ASTKind::Callfunc: {
      auto x = (AST::CallFunc*)ast;

      
    }
  }

  auto ret = EvaluatedResult{ };

  ret.ast = ast;
  ret.type = this->eval_type(ast);

  return ret;
}

ValueType Sema::eval_type(AST::Base* ast) {
  if( !ast )
    return { };

  if( this->caches.contains(ast) )
    return this->caches[ast];

  auto& ret = this->caches[ast];

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
        ret.elems.emplace_back(eval_type(sub));
      }

      break;
    }

    case ASTKind::Argument: {
      auto argument = (AST::Argument*)ast;

      ret = eval_type(argument->type);

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

      if( !eval_type(if_x->cond).equals(ValueType::Kind::Bool) ) {
        Error(ErrorKind::TypeMismatch, if_x->cond, "condition must be boolean")
          .emit();
      }

      ret = eval_type(if_x->if_true);

      if( if_x->if_false && !ret.equals(eval_type(if_x->if_false)) ) {
        Error(ErrorKind::TypeMismatch, if_x->token, "if-expr type mismatch")
          .emit();
      }

      break;
    }

    case ASTKind::For: {
      auto for_x = (AST::For*)ast;

      eval_type(for_x->init);

      if( !eval_type(for_x->cond).equals(ValueType::Kind::Bool) ) {
        Error(ErrorKind::TypeMismatch, for_x->cond, "condition must be boolean").emit();
      }

      eval_type(for_x->counter);
      eval_type(for_x->code);

      break;
    }

    case ASTKind::Return: {
      auto x = (AST::Return*)ast;


      break;
    }

    case ASTKind::Scope: {
      auto scope = (AST::Scope*)ast;

      // 空のスコープ
      if( scope->elements.empty() )
        break;

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

      ret = lhs;
      break;
    }
  }

  return ret;
}



} // namespace metro::semantics

