#include "AST.h"
#include "Error.h"
#include "Utils.h"
#include "MetroDriver/Sema.h"

namespace metro::semantics {

bool Sema::get_type_from_name(ValueType& out, std::string_view name) {
  for( auto&& [s, vt] : ValueType::name_table ) {
    if( name == s ) {
      out = vt;
      return true;
    }
  }

  // todo: find struct

  return false;
}

FunctionInfo* Sema::find_func(std::string_view name) {
  for( auto&& func : this->functions )
    if( func.name == name )
      return &func;

  return nullptr;
}

ScopeInfo& Sema::get_cur_scope() {
  return this->scope_info_map[*this->scope_history.begin()];
}

ScopeInfo& Sema::enter_scope(AST::Scope* ast) {
  auto&& info = this->scope_info_map[ast];

  info.ast = ast;

  this->scope_history.emplace_front(ast);

  return info;
}

void Sema::leave_scope(AST::Scope* ast) {
  if( ast != *this->scope_history.begin() ) {
    crash;
  }

  this->scope_history.pop_front();
}

bool Sema::is_lvalue(AST::Base* ast) {
  if( !ast )
    return false;

  switch( ast->kind ) {
    case ASTKind::Variable:
      return true;
  }

  return false;
}

void Sema::get_last_expr(AST::Base* ast, ASTVector& out) {
  if( !ast )
    return;

  switch( ast->kind ) {
    case ASTKind::Type:
    case ASTKind::Argument:
    case ASTKind::VarDefine:
    case ASTKind::Function:
    case ASTKind::Struct:
      break;

    case ASTKind::Scope: {
      auto x = (AST::Scope*)ast;

      if( x->elements.empty() )
        out.emplace_back(ast);
      else
        get_last_expr(*((AST::Scope*)ast)->elements.rbegin(), out);

      break;
    }

    case ASTKind::If: {
      auto x = (AST::If*)ast;

      get_last_expr(x->if_true, out);
      get_last_expr(x->if_false, out);

      break;
    }

    case ASTKind::For:
    case ASTKind::Loop:
    case ASTKind::While:
      TODO_IMPL

    default:
      out.emplace_back(ast);
      break;
  }
}

ASTVector Sema::get_returnable_expr(AST::Base* ast) {
  ASTVector ret;

  // "return"
  ast_map(
    ast,
    [&ret] (AST::Base* ast) { if( ast->kind == ASTKind::Return ) ret.emplace_back(ast); }
  );

  // last
  get_last_expr(ast, ret);

  // remove duplicates
  ret.erase(std::unique(ret.begin(), ret.end()), ret.end());

  return ret;
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

} // namespace metro::semantics
