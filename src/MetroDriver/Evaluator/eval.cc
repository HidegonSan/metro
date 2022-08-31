#include "MetroDriver/evaluator.h"
#include "GC.h"

namespace metro {

Object** Evaluator::eval_lvalue(AST::Base* ast) {
  using ASTKind = AST::Kind;

  switch( ast->kind ) {
    case ASTKind::Variable: {
      auto x = (AST::Variable*)ast;

      switch( x->defined->kind ) {
        case ASTKind::Argument:
          return &((AST::Argument*)x->defined)->value;

        case ASTKind::VarDefine:
          return &((AST::VarDefine*)x->defined)->value;
      }

      break;
    }
  }

  crash;
}

Object* Evaluator::eval(AST::Base* ast) {
  using AST::Kind;

  if( !ast ) {
    return Object::none;
  }

  // Object* ret = Object::none;
  Object* ret = nullptr;

  switch( ast->kind ) {
    case Kind::None:
    case Kind::Function:
    case Kind::Struct:
      break;

    case Kind::Boolean: {
      ret = gcnew(ValueType::Kind::Bool);

      ret->v_bool = ((AST::Boolean*)ast)->val;

      break;
    }

    case Kind::Value: {
      ret = ((AST::Value*)ast)->object;
      break;
    }

    case Kind::Variable:
      ret = *eval_lvalue(ast);
      break;
    /* {
      auto x = (AST::Variable*)ast;

      switch( x->defined->kind ) {
        case Kind::VarDefine:
          ret = ((AST::VarDefine*)x->defined)->value;
          break;

        case Kind::Argument:
          ret = ((AST::Argument*)x->defined)->value;
          break;
      }

      break;
    }*/

    case Kind::Callfunc: {
      auto x = (AST::CallFunc*)ast;

      if( x->callee_builtin ) {
        std::vector<Object*> args;

        for( auto&& arg : x->args ) {
          args.emplace_back(eval(arg)->clone());
        }

        ret = x->callee_builtin->func(args);
        break;
      }

      //std::vector<Object*> args_bak;

      auto callee = x->callee_func;

      enter_func(callee);

      alert;

      // for( size_t i = 0; i < x->args.size(); i++ ) {
      //   args_bak.emplace_back(callee->args[i].value);
      //   callee->args[i].value = eval(x->args[i]);
      // }

      ret = eval(callee->code);

      // for( size_t i = 0; i < x->args.size(); i++ ) {
      //   callee->args[i].value = args_bak[i];
      // }

      leave_func();

      break;
    }

    case Kind::Compare:
      return compute_compare((AST::Compare*)ast);

    case Kind::Assign: {
      auto x = (AST::Expr*)ast;

      auto obj = eval_lvalue(x->lhs);
      auto rhs = eval(x->rhs);

      //(*obj)->ref_count--;
      *obj = rhs;

      return rhs;
    }

    case Kind::If: {
      auto x = (AST::If*)ast;
      auto cond = eval(x->cond);

      if( cond->v_bool ) {
        ret = eval(x->if_true);
      }
      else if( x->if_false ) {
        ret = eval(x->if_false);
      }

      break;
    }

    case Kind::For: {
      auto x = (AST::For*)ast;

      eval(x->init);

      while( eval(x->cond)->v_bool ) {
        eval(x->code);
        eval(x->counter);
      }

      break;
    }

    case Kind::Return: {
      auto x = (AST::Return*)ast;

      if( x->expr )
        ret = this->eval(x->expr);

      this->cur_call_stack->is_returned = true;

      break;
    }

    case Kind::VarDefine: {
      auto x = (AST::VarDefine*)ast;

      Object* obj = nullptr;

      // let x : T;
      if( !x->init && x->type ) {
        obj = construct_object_from_type(x->type);
      }

      auto val = eval(x->init);

      if( x->value != val ) {
        // if( x->value ) {
        //   x->value->ref_count--;
        //   val->ref_count++;
        // }

        x->value = val;
      }

      break;
    }

    case Kind::Scope: {
      auto x = (AST::Scope*)ast;

      this->enter_scope(x);

      for( auto&& item : x->elements ) {
        ret = this->eval(item);

        if( this->cur_call_stack->is_returned )
          break;
      }



      this->leave_scope();

      break;
    }

    default: {
      auto x = (AST::Expr*)ast;

      ret = eval_operator(x->kind, eval(x->lhs)->clone(), eval(x->rhs));

      break;
    }
  }

  if( !ret ) {
    return Object::none;
  }

  return ret;
}

} // namespace metro
