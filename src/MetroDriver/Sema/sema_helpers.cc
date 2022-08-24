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

AST::Base* Sema::find_var_defined(std::string_view name) {
  for( auto&& ctx : scopelist ) {
    for( int64_t i = ctx.cur_index; i >= 0; i-- ) {
      auto& x = ctx.scope->elems[i];

      if( x->kind == ASTKind::VarDefine && ((AST::VarDefine*)x)->name == name ) {
        return x;
      }
    }
  }

  if( cur_func_ast != nullptr ) {
    for( auto&& arg : cur_func_ast->args ) {
      if( arg.name == name ) {
        return &arg;
      }
    }
  }

  return nullptr;
}

Sema::VariableContext* Sema::find_var_context(AST::Base* defined) {
  for( auto&& ctx : scopelist ) {
    for( auto&& pair : ctx.var_context ) {
      if( pair.second.defined == defined ) {
        return &pair.second;
      }
    }
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

ASTVector Sema::find_return(AST::Base* ast) {
  ASTVector ret;

  switch( ast->kind ) {
    case AST::Kind::Return: {
      ret.emplace_back(ast);
      break;
    }

    case AST::Kind::Scope: {
      auto ast_scope = (AST::Scope*)ast;

      for( auto&& x : ast_scope->elems ) {
        append_vec(ret, find_return(x));
      }

      break;
    }

    case AST::Kind::Function: {
      auto ast_func = (AST::Function*)ast;

      append_vec(ret, find_return(ast_func->code));

      break;
    }
  }

  return ret;
}

ASTVector Sema::get_all_final_expr(AST::Base* ast) {
  // if( !ast )
  //   return { };
  assert(ast != nullptr);

  ASTVector ret;

  switch( ast->kind ) {
    case AST::Kind::Function:
      break;

    case AST::Kind::If: {
      auto if_ast = (AST::If*)ast;

      append_vec(ret, get_all_final_expr(if_ast->if_true));
      append_vec(ret, get_all_final_expr(if_ast->if_false));

      break;
    }

    case AST::Kind::Scope: {
      auto ast_scope = (AST::Scope*)ast;

      if( ast_scope->elems.empty() ) {
        ret.emplace_back(ast);
        break;
      }

      append_vec(ret, get_all_final_expr(*ast_scope->elems.rbegin()));
      break;
    }

    default:
      ret.emplace_back(ast);
      break;
  }

  return ret;
}

ASTVector Sema::get_final_expr_full(AST::Base* ast) {
  ASTVector ret;

  append_vec(ret, find_return(ast));
  append_vec(ret, get_all_final_expr(ast));

  debug(
    auto s = ret.size();

    ret.erase(std::unique(ret.begin(), ret.end()), ret.end());

    assert(s == ret.size());
  )

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
