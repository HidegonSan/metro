#include "AST.h"
#include "Sema/Sema.h"
#include "Error.h"
#include "Debug.h"
#include "Utils.h"

namespace Metro::Semantics {
  ValueType Sema::sema_callfunc(AST::CallFunc* ast) {
    bool is_builtin = false;
    std::vector<ValueType> arg_types;

    for( auto&& arg : ast->args ) {
      arg_types.emplace_back(walk(arg));
    }

    if( (ast->callee = find_func(ast->name)) == nullptr ) {
      if( (ast->callee_builtin = find_builtin_func(ast->name)) == nullptr ) {
        Error::add_error(ErrorKind::Undefined, ast->token, "undefined function name");
        Error::exit_app();
      }

      is_builtin = true;
    }

    auto arg = arg_types.begin();
    size_t max = is_builtin ? ast->callee_builtin->arg_types.size() : ast->callee->args.size();

    for( size_t i = 0;; i++, arg++ ) {
      if( i == max ) {
        if( i != ast->args.size() ) {
          Error::add_error(ErrorKind::TooManyArguments, ast->token, "too many arguments");
        }

        break;
      }

      auto&& func_arg_type = is_builtin ? ast->callee_builtin->arg_types[i] : walk(ast->callee->args[i].type);

      if( func_arg_type.equals_kind(ValueType::Kind::Args) ) {
        break;
      }

      if( i == ast->args.size() ) {
        Error::add_error(ErrorKind::TooFewArguments, ast->token, "too few arguments");
        break;
      }

      auto arg_ast = ast->args[i];

      if( !func_arg_type.equals(*arg) ) {
        Error::add_error(ErrorKind::TypeMismatch, arg_ast, "type mismatch");
      }
    }

    return is_builtin ? ast->callee_builtin->ret_type : walk(ast->callee);
  }

  ValueType Sema::sema_controls(AST::Base* ast) {
    ValueType ret;

    switch( ast->kind ) {
      case ASTKind::If: {
        auto if_x = (AST::If*)ast;

        if( !walk(if_x->cond).equals(ValueType::Kind::Bool) ) {
          Error::add_error(ErrorKind::TypeMismatch, if_x->cond, "condision must be boolean");
        }

        ret = walk(if_x->if_true);

        if( if_x->if_false&&!ret.equals(walk(if_x->if_false)) ) {
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