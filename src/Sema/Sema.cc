#include "Types.h"
#include "MetroDriver/Evaluator.h"
#include "Sema/Sema.h"
#include "Error.h"
#include "Debug.h"
#include "Utils.h"

namespace Metro::Semantics {

  // initialize Sema
  void Sema::init(AST::Scope* root) {
    this->root = root;

    // append all functions to functions
    for( auto scope = (AST::Scope*)root; auto&& x : scope->elems ) {
      if( x->kind == ASTKind::Function ) {
        functions.emplace_back((AST::Function*)x);
      }
    }
  }

  ValueType Sema::walk(AST::Base* ast) {
    if( !ast ) {
      return { };
    }

    if( walked.contains(ast) ) {
      return walked[ast];
    }

    auto& ret = walked[ast];

    switch( ast->kind ) {
      case ASTKind::Type: {
        auto type = (AST::Type*)ast;

        if( type->name == "int" ) {
          ret = ValueType::Kind::Int;
        }
        else {
          TODO_IMPL
        }

        break;
      }

      case ASTKind::Argument: {
        auto argument = (AST::Argument*)ast;

        ret = walk(argument->type);

        break;
      }

      case ASTKind::Value: {
        auto val = (AST::Value*)ast;

        val->object = create_obj(ast->token);

        ret = val->object->type;
        break;
      }

      case ASTKind::Callfunc: {
        auto callfunc = (AST::CallFunc*)ast;



        break;
      }

      // ------------------- //
      // ------------------- //
      case ASTKind::Function: {

        auto func = (AST::Function*)ast;

        std::vector<ReturnTypeElement> ret_elems;

        std::vector<AST::Base*> lastexpr_list;

        cfn_ast = func;
        cfn_ret_list = &ret_elems;

        // arguments
        for(auto&&x:func->args){
          walk(&x);
        }

        // return type
        auto rettype = walk(func->return_type);

        get_lastval_full(lastexpr_list, func->code);

        alert;

        // Analyze return-type
        for(size_t ix = 0; auto&& last :lastexpr_list){
          std::cout<<last->to_string()<<std::endl;

          ValueType tmp;

          // Expr and contain selfcall
          if( last->is_expr && contains_callfunc_in_expr(func->name, last) ) {
            
          }
          else {
            tmp = walk(last);
          }
        }

        // code
        walk(func->code);
        
        break;
      }
      
      case ASTKind::Scope: {
        auto scope = (AST::Scope*)ast;

        if( scope->elems.empty() ) {
          break;
        }

        auto iter = scope->elems.begin();
        auto last = scope->elems.end() - 1;

        for( ; iter != last; iter++ ) {
          walk(*iter);
        }

        ret = walk(*last);
        break;
      }

      default: {
        TODO_IMPL
      }
    }

    return ret;
  }

  // return 式を探して out に追加する
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

  // 式もしくは結果になりうる全ての式を out に追加する
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

        //out.emplace_back(*ast_scope->elems.rbegin());
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

  // pass AST::Expr !!
  bool Sema::contains_callfunc_in_expr(std::string_view name, AST::Base* ast) {
    
    if( ast->kind == ASTKind::Callfunc ) {
      return ((AST::CallFunc*)ast)->name == name;
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