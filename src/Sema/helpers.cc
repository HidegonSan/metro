#include "AST.h"
#include "Sema/Sema.h"
#include "Debug.h"
#include "Utils.h"

namespace Metro::Semantics {
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

  void Sema::find_return(std::vector<AST::Base*>& out, AST::Base* ast) {
    switch( ast->kind ) {
      case AST::Kind::Return: {
        out.emplace_back(ast);
        break;
      }

      case AST::Kind::Scope: {
        auto ast_scope = (AST::Scope*)ast;

        for( auto&& x : ast_scope->elems ) {
          find_return(out, x);
        }

        break;
      }

      case AST::Kind::Function: {
        auto ast_func = (AST::Function*)ast;

        find_return(out, ast_func->code);

        break;
      }
    }
  }

  void Sema::get_lastvalues(std::vector<AST::Base*>& out, AST::Base* ast) {
    if( !ast ) {
      return;
    }

    switch( ast->kind ) {
      case AST::Kind::Function:
        break;

      case AST::Kind::If: {
        auto if_ast = (AST::If*)ast;

        get_lastvalues(out, if_ast->if_true);
        get_lastvalues(out, if_ast->if_false);

        break;
      }

      case AST::Kind::Scope: {
        auto ast_scope = (AST::Scope*)ast;

        if( ast_scope->elems.empty() ) {
          out.emplace_back(ast);
          break;
        }

        get_lastvalues(out, *ast_scope->elems.rbegin());
        break;
      }

      default:
        out.emplace_back(ast);
        break;
    }
  }

  void Sema::get_lastval_full(std::vector<AST::Base*>& out, AST::Base* ast) {
    find_return(out, ast);
    get_lastvalues(out, ast);

    std::map<AST::Base*, int> tmp;

    for( auto&& x : out ) {
      tmp[x] = 0;
    }

    out.clear();

    for( auto&& x : tmp ) {
      out.emplace_back(x.first);
    }
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

      case TokenKind::String: {
        obj->type = ValueType::Kind::String;
        obj->v_str = Utils::Strings::to_u16string(std::string(token->str));
        break;
      }

      default:
        crash;
    }

    return obj;
  }

}