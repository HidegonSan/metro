#pragma once

#include <map>
#include <list>
#include <vector>
#include "AST.h"

#include "Utils.h"

namespace Metro::Semantics {

  class Analyzer {
  public:
    struct Scope;

    struct TypeInfo {
      enum class Condition {
        NotInferred,
        Inferred,
      };

      Scope*     scope;
      Condition  cond;
      ValueType  val_type;

      TypeInfo()
        : scope(nullptr),
          cond(Condition::NotInferred)
      {
      }

      TypeInfo(ValueType&& vt)
        : cond(Condition::Inferred),
          val_type(vt)
      {
      }

      TypeInfo(ValueType const& vt)
        : cond(Condition::Inferred),
          val_type(vt)
      {
      }
    };

    struct VarInfo {
      std::string_view  name;
      TypeInfo  type;
    };

    struct Scope {
      Scope*  owner;
      AST::Base*  ast;
      std::vector<VarInfo> var_list;
      std::vector<Scope> scopes;
      size_t index;

      Scope()
        : owner(nullptr),
          ast(nullptr),
          index(0)
      {
      }

      Scope(Scope&&) = delete;
      Scope(Scope const&) = delete;
    };

    explicit Analyzer(AST::Base* top)
      : cur_scope(&root_scope),
        top_ast(top)
    {
    }

    TypeInfo infer(AST::Base* ast) {
      if( !ast ) {
        return { };
      }

      if( caches.contains(ast) ) {
        return caches[ast];
      }

      auto& ret = caches[ast];

      switch( ast->kind ) {
        case AST::Kind::Value: {
          auto x = (AST::Value*)ast;

          x->object = create_obj(ast->token);
          ret = x->object->type;

          break;
        }

        case AST::Kind::Variable: {
          

          break;
        }

        case AST::Kind::MemberAccess: {

          break;
        }

        case AST::Kind::Scope: {

          break;
        }

        case AST::Kind::Function: {
          auto x = (AST::Function*)ast;

          func_ctx_list.emplace_front(x);
          
          std::vector<AST::Base*> lastnodes;
          std::vector<std::tuple<AST::Base*, bool, TypeInfo>> lastnode_types;

          auto fun = [&, this, yx = x] (AST::Base* ast) -> std::pair<bool, TypeInfo> {
            if( ast->kind == AST::Kind::Callfunc && yx->name == ((AST::CallFunc*)ast)->name ) {
              return { false, { } };
            }

            return { true, infer(ast) };
          };

          get_last_nodes(lastnodes, x->code);

          for( auto&& nd : lastnodes ) {
            auto [res, t] = fun(nd);

            lastnode_types.emplace_back(nd, res, t);
          }

          func_ctx_list.pop_front();

          break;
        }

        case AST::Kind::Struct: {

          break;
        }

        default: {
          if( ast->kind < AST::Kind::Mul || AST::Kind::Sub < ast->kind ) {
            crash;
          }

          auto x = (AST::Expr*)ast;

          auto lhs = infer(x->lhs);
          auto rhs = infer(x->rhs);

          // todo

          ret = lhs;
          break;
        }
      }

      return ret;
    }

    TypeInfo& infer_lval(AST::Base* ast) {

    }

  private:

    struct func_context {
      AST::Function* ast;
      std::vector<AST::Base*> lastnodes;

      explicit func_context(AST::Function* ast)
        : ast(ast)
      {
      }

      bool is_lastnode(AST::Base* x) {
        return std::find(lastnodes.begin(), lastnodes.end(), x) != lastnodes.end();
      }
    };

    static Object* create_obj(Token* token) {
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

    bool is_branchable(AST::Base* ast) {
      switch( ast->kind ) {
        case AST::Kind::If:
          return true;
      }
      
      return false;
    }

    void find_return(std::vector<AST::Base*>& out, AST::Base* ast) {
      if( !ast ) {
        return;
      }

      switch( ast->kind ) {
        case AST::Kind::Return:
          out.emplace_back(ast);
          break;
        
        case AST::Kind::If: {
          auto x = (AST::If*)ast;

          find_return(out, x->if_true);
          find_return(out, x->if_false);

          break;
        }

        case AST::Kind::Scope: {
          auto x = (AST::Scope*)ast;

          for( auto&& nd : x->elems ) {
            find_return(out, nd);
          }

          break;
        }
      }
    }

    void get_last_nodes(std::vector<AST::Base*>& out, AST::Base* ast) {
      if( !ast ) {
        return;
      }
    
      switch( ast->kind ) {
        case AST::Kind::If: {
          auto x = (AST::If*)ast;

          get_last_nodes(out, x->if_true);
          get_last_nodes(out, x->if_false);

          break;
        }

        case AST::Kind::Scope: {
          auto x = (AST::Scope*)ast;

          if( x->elems.empty() ) {
            out.emplace_back(ast);
            break;
          }

          auto it = x->elems.rbegin();

          get_last_nodes(out, *it);

          for( ++it; it != x->elems.rend(); it++ ) {
            find_return(out, *it);
          }

          break;
        }

        case AST::Kind::Return: {
          auto x = (AST::Return*)ast;

          get_last_nodes(out, x->expr);

          break;
        }

        default:
          out.emplace_back(ast);
      }
    }

    void enter_scope(AST::Base* ast) {
      for( auto&& sub : cur_scope->scopes ) {
        if( sub.ast == ast ) {
          cur_scope = &sub;
          return;
        }
      }

      auto& scope = cur_scope->scopes.emplace_back();

      scope.owner = cur_scope;
      scope.ast = ast;

      cur_scope = &scope;
    }

    void leave_scope() {
      cur_scope = cur_scope->owner;
    }

    AST::Base* top_ast;

    Scope  root_scope;
    Scope* cur_scope;

    std::list<func_context> func_ctx_list;

    func_context& get_cur_fctx() {
      return *func_ctx_list.begin();
    }

    std::map<AST::Base*, TypeInfo> caches;
  };

}