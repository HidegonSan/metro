// ----------------------------- //
//  Sema : 意味解析

//  型の一致確認、型推論、など行う
// ----------------------------- //

#pragma once

#include <vector>
#include <list>
#include <map>
#include "AST.h"
#include "colors.h"

#define COL_TYPENAME 

namespace metro {

using TypeVector = std::vector<ValueType>;
using ASTVector = std::vector<AST::Base*>;

class Sema {

public:
  using ASTKind = AST::Kind;
  using WalkerFuncPointer = bool(*)(AST::Base*);

private:
  struct VariableContext {
    std::string_view name;

    ValueType type;
    bool was_type_analyzed;

    size_t index;

    VariableContext()
      : was_type_analyzed(false),
        index(0)
    {
    }
  };

  struct ScopeContext {
    AST::Scope*  ast = nullptr;

    AST::Base* cur_ast = nullptr; //
    size_t  cur_index = 0;        // Sema::walk()

    std::vector<VariableContext> variables;
  };

  struct Function {
    std::vector<VariableContext> arguments;


  };

  struct TypeAttr {
    bool    left;
    bool    right;
    bool    reference;

    TypeAttr()
      : left(0),
        right(0),
        reference(0)
    {
    }
  };

public:
  explicit Sema(AST::Scope* root);

  static Sema* get_instance();

  void analyze();

  // == walk ==
  ValueType walk(AST::Base* ast);

  ValueType sema_factor(AST::Base* ast);

  // sema-parts
  ValueType sema_callfunc(AST::CallFunc* ast);
  ValueType sema_controls(AST::Base* ast);


  // === helpers === //

  TypeAttr get_type_attr(AST::Base* ast);


  //
  void expect_all_same_with(std::vector<AST::Base*> const& vec, ValueType const& type);

  // //
  // // 名前から変数の定義場所を探す
  // AST::Base* find_var_defined(std::string_view name);

  // //
  // // find variable context
  // VariableContext* find_var_context(AST::Base* defined);

  VariableContext* get_var_context(std::string_view name, bool create = false);

  //
  // 名前から関数を探す
  AST::Function* find_func(std::string_view name);

  //
  // 名前から組み込み関数を探す
  BuiltinFunc const* find_builtin_func(std::string_view name);

  //
  // 関数の戻り値の型を解析する
  ValueType& analyze_func_return_type(AST::Function* ast);


  //
  // 式の中に name と同名の関数呼び出しが含まれているかどうか確認
  //  !!! ast には AST::Expr を渡すこと !!!
  //  !!! 'ast' must AST::Expr !!!
  bool contains_callfunc_in_expr(std::string_view name, AST::Base* ast);


  /* TODO: impl */
  static ASTVector&& get_finder_result();
  static void ast_finder(AST::Base* ast, WalkerFuncPointer walker);


  static void get_results(ASTVector& vec, AST::Base* ast, bool last_elem = false);
  static ASTVector get_results_wrap(AST::Base* ast);

  //
  // create object from token
  static Object* create_obj(Token* token);

  //
  //  /*  check argument types of call and callee   */
  //
  // args:
  //   fn_args   : argument types in function
  //   call_args : argument types in call
  //
  // return:
  //   >=0 : position to detected mismatching type
  //    -1 : too few arguments to call
  //    -2 : too many arguments to call
  //    -3 : success
  static int check_arguments(TypeVector const& fn_args, TypeVector const& call_args);

private:

  ScopeContext& get_cur_scope();

  AST::Scope* root;

  AST::Variable* arrow_unini = nullptr;

  // current walking function
  AST::Function* cur_func_ast;

  // all functions in root
  std::vector<AST::Function*> functions;

  // scope-list
  std::list<ScopeContext> scopelist;

  // variable
  // std::map<AST::Base*, VariableContext> var_ctx;

  // caches
  std::map<AST::Base*, ValueType> walked;

};

} // namespace metro
