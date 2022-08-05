#pragma once

#include <vector>
#include "AST.h"

namespace Metro::Semantics {

  struct TypeInfo {

  };

  class Sema {
  public:

    // return 式を探して out に追加する
    void find_return(std::vector<AST::Base*>& out, AST::Base* ast) {
      switch( ast->kind ) {
        case AST::Kind::Return: {
          out.emplace_back(ast);
          break;
        }

        case AST::Kind::Scope: {
          auto ast_scope = (AST::Scope*)ast;

          for( auto&& x : ast_scope->elems ) {
            
          }
        }
      }
    }

    // 式もしくは結果になりうる全ての式を out に追加する
    void get_lastvalues(std::vector<AST::Base*>& out, AST::Base* ast) {
      if( ast ) {
        return;
      }

      switch( ast->kind ) {
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

          out.emplace_back(*ast_scope->elems.rbegin());
          break;
        }

        case AST::Kind::Function: {

        }

        default:
          out.emplace_back(ast);
          break;
      }
    }

  };

}