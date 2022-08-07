#pragma once

#include <vector>
#include <list>
#include <map>
#include "AST.h"

namespace Metro::Semantics {
  struct ScopeContext {
    AST::Scope*  scope = nullptr;
    AST::Base* cur_ast = nullptr;
    size_t  cur_index = 0;
  };

  class Sema {
    using ASTKind = AST::Kind;

  public:

    // initialize Sema
    void init(AST::Scope* root);

    // == walk ==
    ValueType walk(AST::Base* ast);


    //
    // 関数の戻り値の型を解析する
    void analyze_func_return_type(ValueType& out, AST::Function* ast);

    //
    // return 式を探して out に追加する
    void find_return(std::vector<AST::Base*>& out, AST::Base* ast);

    //
    // 全ての last-expr を out に追加する
    // 注意: return は含まれません
    void get_lastvalues(std::vector<AST::Base*>& out, AST::Base* ast);

    //
    // 式の結果になりうる全ての式を out に追加する
    //  last-expr と return どっちも追加される
    void get_lastval_full(std::vector<AST::Base*>& out, AST::Base* ast);

    //
    // 式の中に name と同名の関数呼び出しが含まれているかどうか確認
    //  !!! ast には AST::Expr を渡すこと !!!
    //  !!! 'ast' must AST::Expr !!!
    bool contains_callfunc_in_expr(std::string_view name, AST::Base* ast);

    //
    // create object from token
    static Object* create_obj(Token* token);

  private:

    AST::Scope* root;

    // current walking function
    AST::Function* cfn_ast;

    // all functions in root
    std::vector<AST::Function*> functions;

    // scope-list
    std::list<ScopeContext> scopelist;

    // caches
    std::map<AST::Base*, ValueType> walked;

  };
}