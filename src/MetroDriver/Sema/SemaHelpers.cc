#include <functional>
#include "AST.h"
#include "Error.h"
#include "Utils.h"
#include "MetroDriver/Sema.h"

namespace metro::semantics {

void ast_map(
  AST::Base* ast,
  std::function<void(AST::Base*)> fp,
  std::function<void(AST::Base*)> fp_begin = nullptr,
  std::function<void(AST::Base*)> fp_end = nullptr) {

  if( !ast )
    return;

  if( fp_begin )
    fp_begin(ast);

  fp(ast);

  switch( ast->kind ) {
    case ASTKind::None:
    case ASTKind::Type:
      break;

    case ASTKind::Argument:
      ast_map(((AST::Argument*)ast)->type, fp, fp_begin, fp_end);
      break;

    case ASTKind::Boolean:
    case ASTKind::Value:
    case ASTKind::Variable:
      break;

    case ASTKind::Array:
    case ASTKind::Tuple:
    case ASTKind::Scope: {
      auto x = (AST::ListBase*)ast;

      for( auto&& y : x->elements )
        ast_map(y, fp, fp_begin, fp_end);

      break;
    }

    case ASTKind::Callfunc: {
      auto x = (AST::CallFunc*)ast;

      for( auto&& arg : x->args )
        ast_map(arg, fp, fp_begin, fp_end);

      break;
    }

    case ASTKind::Subscript:
    case ASTKind::MemberAccess:
    case ASTKind::Mul:
    case ASTKind::Div:
    case ASTKind::Add:
    case ASTKind::Sub:
    case ASTKind::Assign: {
      auto x = (AST::Expr*)ast;

      ast_map(x->lhs, fp, fp_begin, fp_end);
      ast_map(x->rhs, fp, fp_begin, fp_end);

      break;
    }

    case ASTKind::Compare: {
      auto x = (AST::Compare*)ast;

      ast_map(x->first, fp, fp_begin, fp_end);

      for( auto&& item : x->list )
        ast_map(item.ast, fp, fp_begin, fp_end);

      break;
    }

    case ASTKind::Return:
      ast_map(((AST::Return*)ast)->expr, fp, fp_begin, fp_end);
      break;

    case ASTKind::If:
      ast_map(((AST::If*)ast)->cond, fp, fp_begin, fp_end);
      ast_map(((AST::If*)ast)->if_true, fp, fp_begin, fp_end);
      ast_map(((AST::If*)ast)->if_false, fp, fp_begin, fp_end);
      break;

    case ASTKind::VarDefine:
      ast_map(((AST::VarDefine*)ast)->type, fp, fp_begin, fp_end);
      ast_map(((AST::VarDefine*)ast)->init, fp, fp_begin, fp_end);
      break;

    case ASTKind::For:
    case ASTKind::Loop:
    case ASTKind::While:
      TODO_IMPL

    case ASTKind::Function: {
      auto x = (AST::Function*)ast;

      for(auto&&arg:x->args)
        ast_map(&arg, fp, fp_begin, fp_end);

      ast_map(x->return_type, fp, fp_begin, fp_end);
      ast_map(x->code, fp, fp_begin, fp_end);

      break;
    }

    default:
      TODO_IMPL
  }

  if( fp_end )
    fp_end(ast);
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

ScopeInfo& Sema::get_cur_scope() {
  return this->scope_info_map[*this->scope_history.begin()];
}

ScopeInfo& Sema::enter_scope(AST::Scope* ast) {
  auto&& info = this->scope_info_map[ast];

  info.ast = ast;

  this->scope_history.emplace_front(ast);

  return info;
}

void Sema::leave_scope() {
  this->scope_history.pop_front();
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

} // namespace metro::semantics
