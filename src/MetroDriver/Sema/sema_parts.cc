#include "AST.h"
#include "MetroDriver/Sema.h"
#include "Error.h"
#include "Debug.h"
#include "Utils.h"

namespace Metro::Semantics {
  ValueType Sema::sema_callfunc(AST::CallFunc* ast) {
    std::vector<ValueType> arg_types;

    // arguments
    for( auto&& arg : ast->args ) {
      arg_types.emplace_back(walk(arg));
    }

    auto find = find_func(ast->name);
    auto is_builtin = find == nullptr;

    BuiltinFunc const* bfun = nullptr;

    // doesn't exists user-defined function
    if( find == nullptr ) {
      // doens't exists built-in function
      if( (bfun = find_builtin_func(ast->name)) == nullptr ) {
        Error::add_error(ErrorKind::Undefined, ast->token, "undefined function name");
        Error::exit_app();
      }
      else {
        ast->callee_builtin = bfun;
      }
    }
    else {
      ast->callee = find;
    }

    auto arg = arg_types.begin();
    size_t max = is_builtin ? bfun->arg_types.size() : find->args.size();

    for( size_t i = 0;; i++, arg++ ) {
      if( i == max ) {
        if( i != ast->args.size() ) {
          Error::add_error(ErrorKind::TooManyArguments, ast->token, "too many arguments");
        }

        break;
      }

      auto&& func_arg_type = is_builtin ? bfun->arg_types[i] : walk(find->args[i].type);

      if( func_arg_type.equals(ValueType::Kind::Args) ) {
        break;
      }

      if( i == ast->args.size() ) {
        Error::add_error(ErrorKind::TooFewArguments, ast->token, "too few arguments");
        break;
      }

      if( !func_arg_type.equals(*arg) ) {
        Error::add_error(ErrorKind::TypeMismatch, ast->args[i], "type mismatch");
      }
    }

    return is_builtin ? bfun->ret_type : walk(find);
  }

  ValueType Sema::sema_controls(AST::Base* ast) {
    ValueType ret;

    switch( ast->kind ) {
      case ASTKind::If: {
        auto if_x = (AST::If*)ast;

        if( !walk(if_x->cond).equals(ValueType::Kind::Bool) ) {
          Error::add_error(ErrorKind::TypeMismatch, if_x->cond, "condition must be boolean");
        }

        ret = walk(if_x->if_true);

        if( if_x->if_false && !ret.equals(walk(if_x->if_false)) ) {
          Error::add_error(ErrorKind::TypeMismatch, if_x, "if-expr type mismatch");
        }

        break;
      }

      case ASTKind::For: {
        auto for_x = (AST::For*)ast;

        walk(for_x->init);

        if( !walk(for_x->cond).equals(ValueType::Kind::Bool) ) {
          Error::add_error(ErrorKind::TypeMismatch, for_x->cond, "condition must be boolean");
        }

        walk(for_x->counter);
        walk(for_x->code);

        break;
      }

      default:
        TODO_IMPL
    }

    return ret;
  }
}