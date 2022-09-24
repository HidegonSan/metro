#include "AST.h"
#include "Error.h"
#include "Utils.h"
#include "MetroDriver/Sema.h"

#define _try_eval_r(_ast) \
  if( auto&& res = this->try_eval_type(_ast); _ast && res.fail() ) \
    return res;

namespace metro::semantics {

Sema::EvalResult Sema::try_eval_type(AST::Base* ast) {
  using Cond = EvalResult::Condition;

  try {
    ast_map(
      ast,
      [&] (AST::Base* ast) {
        switch( ast->kind ) {
          case ASTKind::Variable: {
            auto x = (AST::Variable*)ast;

            if( !this->var_dc_ptr_map[x]->is_deducted ) {
              throw Cond::Incomplete;
            }

            break;
          }

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
              throw Cond::Incomplete;

            break;
          }

          case ASTKind::Assign: {
            auto x = (AST::Expr*)ast;

            auto&& rhs = this->try_eval_type(x->rhs);

            if( !is_lvalue(x->lhs) ) {
              Error(ErrorKind::TypeMismatch, x->lhs, "expected lvalue expression")
                .emit();
            }



            break;
          }

        }
      }
    );
  }
  catch( EvalResult const& res ) {
    return res;
  }
  catch( EvalResult::Condition cond ) {
    return cond;
  }
  catch( ... ) {
    crash;
  }

  return this->eval_type(ast);
}

ValueType Sema::eval_type(AST::Base* ast) {
  if( !ast )
    return { };

  // if( this->caches.contains(ast) )
  //   return this->caches[ast];

  auto& ret = this->caches[ast];

  switch( ast->kind ) {
    case ASTKind::None:
      break;

    case ASTKind::Type: {
      auto type = (AST::Type*)ast;

      assert( this->get_type_from_name(ret, type->name) );

      ret.arr_depth = type->arr_depth;
      ret.have_elements = type->have_elements;

      ret.is_mutable = type->is_mutable;
      ret.is_reference = type->is_reference;

      for( auto&& sub : type->elems )
        ret.elems.emplace_back(this->eval_type(sub));

      alertios(ret.to_string());

      break;
    }

    case ASTKind::Argument: {
      auto argument = (AST::Argument*)ast;

      ret = this->eval_type(argument->type);

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

      ret = this->var_dc_ptr_map[x]->type;

      break;
    }

    case ASTKind::Callfunc: {
      auto x = (AST::CallFunc*)ast;

      auto func = this->find_func(x->name);

      // for( auto&& arg : x->args )
      //   auto&& res = this->eval_type(arg);

      return func->dc.type;
    }

    case ASTKind::Compare: {
      return ValueType::Kind::Bool;
    }

    //
    // assignment
    case ASTKind::Assign: {
      auto x = (AST::Expr*)ast;

      ret = this->eval_type(x->rhs);

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

      this->enter_scope(scope);

      // eval all elements
      // for( auto&& elem : scope->elements )
      //   this->eval_type(elem);

      ret = this->eval_type(*scope->elements.rbegin());

/*
      for( auto it = finals.begin() + 1; it != finals.end(); it++ )
        if( auto&& t = this->eval_type(*it); !ret.equals(t) )
          Error(ErrorKind::TypeMismatch, *it,
            Utils::linkstr( "expected '", ret.to_string(),
              "' but found '", t.to_string(), "'"))
              .emit();
*/

      this->leave_scope(scope);

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

