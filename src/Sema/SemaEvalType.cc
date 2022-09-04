#include "AST.h"
#include "Error.h"
#include "Utils.h"
#include "MetroDriver/Sema.h"

#define _try_eval_r(_ast) \
  if( auto&& res = this->try_eval_type(_ast); _ast && res.fail() ) \
    return res;

namespace metro::semantics {

// --- //

Sema::EvalResult Sema::try_eval_type(AST::Base* ast) {
  using Cond = EvalResult::Condition;

  switch( ast->kind ) {
    case ASTKind::None:
    case ASTKind::Boolean:
    case ASTKind::Value:
    case ASTKind::Variable:
      break;

    case ASTKind::Array:
    case ASTKind::Tuple: {
      auto x = (AST::ListBase*)ast;

      for( auto&& elem : x->elements )
        _try_eval_r(elem);

      break;
    }

    //
    // call function
    case ASTKind::Callfunc: {
      auto x = (AST::CallFunc*)ast;

      auto func = this->find_func(x->name);

      // function not found
      if( !func ) {
        Error(ErrorKind::UndefinedFunction,
          x->token,
          "undefined function name")
            .emit(true);
      }

      // return type is not deducted yet
      if( !func->dc.is_deducted )
        return Cond::Incomplete;

      break;
    }

    case ASTKind::Compare: {
      auto x = (AST::Compare*)ast;

      _try_eval_r(x->first);

      for( auto&& item : x->list )
        _try_eval_r(item.ast);

      break;
    }

    case ASTKind::Return:
      _try_eval_r(((AST::Return*)ast)->expr);
      break;
    
    case ASTKind::If: {
      auto x = (AST::If*)ast;

      _try_eval_r(x->cond);
      _try_eval_r(x->if_true);
      _try_eval_r(x->if_false);

      break;
    }

    case ASTKind::VarDefine: {
      auto x = (AST::VarDefine*)ast;

      _try_eval_r(x->type);
      _try_eval_r(x->init);

      break;
    }

    default: {
      if( !ast->is_expr )
        crash;

      auto x = (AST::Expr*)ast;

      _try_eval_r(x->lhs);
      _try_eval_r(x->rhs);

      break;
    }
  }

  return this->eval_type(ast);
}

ValueType Sema::eval_type(AST::Base* ast) {
  using Cond = EvalResult::Condition;

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
        ret.elems.emplace_back(this->eval_type(sub));
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
      auto x = (AST::CallFunc*)ast;

      auto func = this->find_func(x->name);

      for( auto&& arg : x->args )
        auto&& res = this->eval_type(arg);

      return func->dc.type;
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

      if( !this->eval_type(if_x->cond).equals(ValueType::Kind::Bool) )
        Error(ErrorKind::TypeMismatch, if_x->cond, "condition must be boolean")
          .emit();

      ret = this->eval_type(if_x->if_true);
    
      if( if_x->if_false )
        if( !ret.equals(this->eval_type(if_x->if_false)) )
          Error(ErrorKind::TypeMismatch, if_x->token, "if-expr type mismatch")
            .emit();

      break;
    }

    case ASTKind::For: {
      auto x = (AST::For*)ast;

      this->eval_type(x->init);

      auto&& cond = this->eval_type(x->cond);

      if( !cond.equals(ValueType::Kind::Bool) )
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

      ret = this->eval_type(*scope->elements.begin());

      for( auto it = finals.begin() + 1; it != finals.end(); it++ )
        if( auto&& t = this->eval_type(*it); !ret.equals(t) )
          Error(ErrorKind::TypeMismatch, *it,
            Utils::linkstr( "expected '", ret.to_string(),
              "' but found '", t.to_string(), "'"))
              .emit();

      break;
    }

    case ASTKind::Function:
    case ASTKind::Struct:
      break;

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

