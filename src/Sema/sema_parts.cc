#include "AST.h"
#include "Sema/Sema.h"
#include "Error.h"
#include "Debug.h"
#include "Utils.h"

namespace Metro::Semantics {
  ValueType Sema::sema_callfunc(AST::CallFunc* ast) {
    // TODO: check argument types

    std::vector<ValueType> arg_types;

    for( auto&& arg : ast->args ) {
      walk(arg);
    }

    if( (ast->callee = find_func(ast->name)) == nullptr ) {
      if( (ast->callee_builtin = find_builtin_func(ast->name)) == nullptr ) {
        Error::add_error(ErrorKind::Undefined, ast->token, "undefined function name");
        Error::exit_app();
      }

      return ast->callee_builtin->ret_type;
    }

    return walk(ast->callee);
  }

  ValueType Sema::sema_controls(AST::Base* ast) {
    ValueType ret;

    switch( ast->kind ) {
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

      default:
        TODO_IMPL
    }

    return ret;
  }
}