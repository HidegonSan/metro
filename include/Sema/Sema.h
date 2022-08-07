#pragma once

#include <vector>
#include <map>
#include "AST.h"

namespace Metro::Semantics {

  class Sema {
    using ReturnTypeElement = std::tuple<size_t, AST::Base*, ValueType>;

    std::vector<AST::Function*> func_list;

    AST::Base* root;
    AST::Base* cur_func_ast;

  public:

    void init(AST::Base* root) {
      this->root = root;


    }

    ValueType walk(AST::Base* ast) {

    }

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

    // 式もしくは結果になりうる全ての式を out に追加する
    void get_lastvalues(std::vector<AST::Base*>& out, AST::Base* ast) {
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

          out.emplace_back(*ast_scope->elems.rbegin());
          break;
        }

        default:
          out.emplace_back(ast);
          break;
      }
    }

    void get_lastval_full(std::vector<AST::Base*>& out, AST::Base* ast) {
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


  };

}