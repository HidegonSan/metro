#pragma once

#include <vector>
#include <list>
#include <map>
#include "AST.h"

namespace Metro::Semantics {
  class Sema {
    using ASTKind = AST::Kind;

    struct VariableContext {
      AST::Let*  defined;

      bool was_type_analyzed = false;
      ValueType type;

    };

    struct ScopeContext {
      AST::Scope*  scope = nullptr;
      AST::Base* cur_ast = nullptr;
      size_t  cur_index = 0;

      std::map<std::string_view, VariableContext> var_context;
    };

  public:

    // initialize Sema
    void init(AST::Scope* root);

    // == walk ==
    ValueType walk(AST::Base* ast);

    // sema-parts
    ValueType sema_callfunc(AST::CallFunc* ast);
    ValueType sema_controls(AST::Base* ast);

    

    //
    // 名前から変数の定義場所を探す
    AST::Base* find_var_defined(std::string_view name);

    //
    // find variable context
    VariableContext* find_var_context(AST::Base* defined);

    //
    // 名前から関数を探す
    AST::Function* find_func(std::string_view name);

    //
    // 名前から組み込み関数を探す
    BuiltinFunc const* find_builtin_func(std::string_view name);

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

    ScopeContext& get_cur_scope();

    AST::Scope* root;

    // current walking function
    AST::Function* cfn_ast;

    // all functions in root
    std::vector<AST::Function*> functions;

    // scope-list
    std::list<ScopeContext> scopelist;

    // variable
    // std::map<AST::Base*, VariableContext> var_ctx;

    // caches
    std::map<AST::Base*, ValueType> walked;

  };
}