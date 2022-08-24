#include "AST.h"
#include "MetroDriver/sema.h"
#include "Error.h"
#include "Debug.h"
#include "Utils.h"

namespace metro {

// -------------------
//  関数呼び出し
// -------------------
ValueType Sema::sema_callfunc(AST::CallFunc* ast) {

  std::vector<ValueType> arg_types; // call


  // arguments
  for( auto&& arg : ast->args ) {
    arg_types.emplace_back(walk(arg));
  }

  BuiltinFunc const* bfun;

  auto find = find_func(ast->name);
  auto is_builtin = find == nullptr;

  // doesn't exists user-defined function
  if( find == nullptr ) {
    // doens't exists built-in function
    if( (bfun = find_builtin_func(ast->name)) == nullptr )
      Error(ErrorKind::Undefined, ast->token, "undefined function name").emit(true);
    else
      ast->callee_builtin = bfun;
  }
  else {
    ast->callee_func = find;
  }

  int check_res;

  if( is_builtin ) {
    check_res = check_arguments(bfun->arg_types, arg_types);
  }
  else {
    TypeVector tmp;

    for( auto&& x : find->args ) {
      tmp.emplace_back(walk(x.type));
    }

    check_res = check_arguments(tmp, arg_types);
  }

  switch( check_res ) {
    case -1:
      Error(ErrorKind::TooFewArguments, ast, "too few arguments to call").emit();
      break;

    case -2:
      Error(ErrorKind::TooManyArguments, ast, "too many arguments to call").emit();
      break;

    case -3:
      break;

    default:
      Error(ErrorKind::TypeMismatch, ast->args[check_res], "type mismatch").emit();
      break;
  }

  return is_builtin ? bfun->ret_type : walk(find);
}

ValueType Sema::sema_controls(AST::Base* ast) {
  ValueType ret;

  switch( ast->kind ) {
    case ASTKind::If: {
      auto if_x = (AST::If*)ast;

      if( !walk(if_x->cond).equals(ValueType::Kind::Bool) ) {
        Error(ErrorKind::TypeMismatch, if_x->cond, "condition must be boolean").emit();
      }

      ret = walk(if_x->if_true);

      if( if_x->if_false && !ret.equals(walk(if_x->if_false)) ) {
        Error(ErrorKind::TypeMismatch, if_x, "if-expr type mismatch").emit();
      }

      break;
    }

    case ASTKind::For: {
      auto for_x = (AST::For*)ast;

      walk(for_x->init);

      if( !walk(for_x->cond).equals(ValueType::Kind::Bool) ) {
        Error(ErrorKind::TypeMismatch, for_x->cond, "condition must be boolean").emit();
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

} // namespace metro
