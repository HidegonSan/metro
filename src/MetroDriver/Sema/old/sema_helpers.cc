#include "AST.h"
#include "MetroDriver/sema.h"
#include "Debug.h"
#include "Utils.h"
#include "Error.h"

namespace metro {

inline auto& append_vec(auto&&... e) {
  return Utils::append_vec<AST::Base*>(e...);
}

int Sema::check_arguments(TypeVector const& fn_args, TypeVector const& call_args) {
  if( fn_args.empty() ) {
    if( !call_args.empty() )
      return -2;
  }
  else if( fn_args.rbegin()->kind == ValueType::Kind::Args ) {
    if( call_args.size() < (signed)fn_args.size() - 1 )
      return -2;
  }
  else if( fn_args.size() < call_args.size() ) {
    return -2;
  }
  else if( fn_args.size() > call_args.size() ) {
    return -1;
  }

  size_t i = 0;

  for( auto it = fn_args.begin(); auto&& arg : call_args ) {
    if( it == fn_args.end() || it->kind == ValueType::Kind::Args )
      break;

    if( ! it++ ->equals(arg) )
      return i;

    i++;
  }

  return -3;
}

Sema::TypeAttr Sema::get_type_attr(AST::Base* ast) {
  // TODO

  TypeAttr attr;

  if( ast->kind == AST::Kind::Variable ) {
    attr.left = true;
  }

  return attr;
}

void Sema::expect_all_same_with(std::vector<AST::Base*> const& vec, ValueType const& _expect) {
  for( auto&& ast : vec ) {
    auto&& elem_type = walk(ast);

    if( !elem_type.equals(_expect) ) {
      // Error::add_error(ErrorKind::TypeMismatch, ast,
      //   Utils::linkstr("expected '", type.to_string(), "', but found '", tmp.to_string(), "'")
      // );

      Error(
        ErrorKind::TypeMismatch, ast,
        Utils::linkstr("expected '", _expect.to_string(), "', but found '", elem_type.to_string(), "'")
      )
        .emit();
    }
  }

  Error::check();
}

Sema::VariableContext* Sema::get_var_context(std::string_view name, bool create) {
  for( auto&& scope : this->scopelist ) {
    for( auto&& var : scope.variables ) {
      if( var.name == name )
        return &var;
    }
  }

  if( create ) {
    auto& scope = this->get_cur_scope();
    auto& var = scope.variables.emplace_back();

    var.name = name;
    var.index = scope.variables.size() - 1;

    return &var;
  }

  return nullptr;
}

AST::Function* Sema::find_func(std::string_view name) {
  for( auto&& func : functions) {
    if( func->name == name ) {
      return func;
    }
  }

  return nullptr;
}

BuiltinFunc const* Sema::find_builtin_func(std::string_view name) {
  for( auto&& bifun : BuiltinFunc::builtin_functions ) {
    if( bifun.name == name ) {
      return &bifun;
    }
  }

  return nullptr;
}

/*

{
  if a {
    return 1;
  }
  else {
    return "aiueo";
  }

  if true { 123 } else { 3 }
}

1
"aiueo"
123
3

*/

void Sema::get_results(ASTVector& vec, AST::Base* ast, bool last_elem) {
  if( !ast )
    return;

  if( last_elem && ast->is_expr ) {
    vec.emplace_back(ast);
    return;
  }

  switch( ast->kind ) {
    case ASTKind::None:
    case ASTKind::Boolean:
    case ASTKind::Value:
    case ASTKind::Variable:
    case ASTKind::Array:
    case ASTKind::Tuple:
    case ASTKind::Callfunc:
    case ASTKind::Compare:
      if( last_elem )
        vec.emplace_back(ast);

      break;

    case ASTKind::Scope: {
      auto x = (AST::Scope*)ast;

      if( last_elem && x->elements.empty() ) {
        vec.emplace_back(ast);
        break;
      }

      for( auto&& elem : x->elements ) {
        get_results(vec, elem, last_elem && *x->elements.rbegin() == elem);
      }

      break;
    }

    case ASTKind::If: {
      auto x = (AST::If*)ast;

      get_results(vec, x->if_true, last_elem);
      get_results(vec, x->if_false, last_elem);

      break;
    }

    case ASTKind::Return: {
      auto x = (AST::Return*)ast;

      if( x->expr )
        get_results(vec, x->expr, true);
      else
        vec.emplace_back(ast);

      break;
    }

    case ASTKind::Function:
    case ASTKind::VarDefine:
      break;

    default:
      alertfmt("%d", ast->kind);
      crash;
  }
}

ASTVector Sema::get_results_wrap(AST::Base* ast) {
  ASTVector ret;

  get_results(ret, ast, true);

  return ret;
}

bool Sema::contains_callfunc_in_expr(std::string_view name, AST::Base* ast) {
  if( ast->kind == ASTKind::Callfunc ) {
    return ((AST::CallFunc*)ast)->name == name;
  }

  if( !ast->is_expr ) {
    return false;
  }

  auto x = (AST::Expr*)ast;

  return contains_callfunc_in_expr(name, x->lhs)
    && contains_callfunc_in_expr(name, x->rhs);
}

Object* Sema::create_obj(Token* token) {
  auto obj = new Object;

  switch( token->kind ) {
    case TokenKind::Int: {
      obj->type = ValueType::Kind::Int;
      obj->v_int = atoi(token->str.data());
      break;
    }

    case TokenKind::Float: {
      obj->type = ValueType::Kind::Float;
      obj->v_float = atof(token->str.data());
      break;
    }

    case TokenKind::String: {
      obj->type = ValueType::Kind::String;
      obj->v_str = Utils::Strings::to_u16string(std::string(token->str));

      obj->v_str.erase(obj->v_str.begin());
      obj->v_str.pop_back();

      break;
    }

    default:
      TODO_IMPL
  }

  return obj;
}

Sema::ScopeContext& Sema::get_cur_scope() {
  return *scopelist.begin();
}

} // namespace metro
