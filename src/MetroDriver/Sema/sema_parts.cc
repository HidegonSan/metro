#include "AST.h"
#include "MetroDriver/sema.h"
#include "Error.h"
#include "Debug.h"
#include "Utils.h"

namespace metro {

using ArgList = std::vector<ValueType>;

// -------------------
// check_arguments
//   引数が適切かどうか確認する
//
// args:
//   list1  = arguments that defined at callee
//   list2  = call
//
// return code:
//  >=0: index to mismatched type
//   -1: success
//   -2: list2 is too few to call
//   -3: list2 is too many to call
// -------------------
int check_arguments(ArgList const& list1, ArgList const& list2) {
  int index = 0;

  for( auto i = list1.begin(), j = list2.begin();; index++ ) {
    // i = defined
    // j = call

    if( i == list1.end() ) {
      if( j != list2.end() )
        return -3;

      break;
    }
    else if( i->equals(ValueType::Kind::Args) ) {
      break;
    }
    else if( j == list2.end() ) {
      return -2;
    }

    if( !i->equals(*j) )
      return index;
  }

  return -1;
}


// -------------------
//  関数呼び出し
// -------------------
ValueType Sema::sema_callfunc(AST::CallFunc* ast) {
  std::vector<ValueType> arg_types;

  // arguments
  for( auto&& arg : ast->args ) {
    arg_types.emplace_back(walk(arg));
  }

  std::string_view name;

  AST::Function* user_func;
  BuiltinFunc const* builtin;

  if( ast->expr->kind == AST::Kind::Variable ) {
    
  }

  auto find = find_func(ast->name);
  auto is_builtin = find == nullptr;


  // doesn't exists user-defined function
  if( find == nullptr ) {
    // doens't exists built-in function
    if( (bfun = find_builtin_func(ast->name)) == nullptr ) {
      // Error::add_error(ErrorKind::Undefined, ast->token, "undefined function name");
      // Error::exit_app();

      Error(ErrorKind::Undefined, ast->token, "undefined function name").emit(true);
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
        // Error::add_error(ErrorKind::TooManyArguments, ast->token, "too many arguments");
        Error(ErrorKind::TooManyArguments, ast->token, "too many arguments").emit();
      }

      break;
    }

    auto&& func_arg_type = is_builtin ? bfun->arg_types[i] : walk(find->args[i].type);

    if( func_arg_type.equals(ValueType::Kind::Args) ) {
      break;
    }

    if( i == ast->args.size() ) {
      Error(ErrorKind::TooFewArguments, ast->token, "too few arguments").emit();
      break;
    }

    if( !func_arg_type.equals(*arg) ) {
      Error(ErrorKind::TypeMismatch, ast->args[i], "type mismatch").emit();
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
