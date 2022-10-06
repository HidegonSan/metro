#pragma once

#include <list>
#include <map>
#include <vector>
#include <memory>
#include <stdexcept>
#include <concepts>
#include <functional>

#include "Utils.h"

#if METRO_DEBUG
  #include "AST.h"
#endif

namespace metro {

struct Token;
struct Object;
struct ValueType;

class Error;

namespace AST {

enum class Kind;

struct Base;
struct Type;
struct Argument;
struct Variable;

struct VarDefine;
struct Scope;
struct Function;

} // namespace metro::AST

namespace semantics {

using ASTKind = AST::Kind;
using ASTVector = std::vector<AST::Base*>;
using TypeVector = std::vector<ValueType>;

template <class T>
struct TypeCandidates {
  T* ast;
  std::vector<AST::Base*> candidates;

  bool is_deducted{ };
  AST::Type* specified_type;

  ValueType type;

#if METRO_DEBUG
  virtual std::string to_string() const {
    return Utils::linkstr(
      "{ TypeCandidates<", typeid(T).name(), "> at ", (void*)this, ": ",
      "ast=", (void*)ast, ", ",
      "candidates={",
        Utils::join(", ", candidates, [](auto&x){return Utils::format("%p", x);}),
        "}, ",
      "is_deducted=", is_deducted, ", ",
      "specified_type=",
        specified_type ? specified_type->to_string() : "null", ", ",
      "type=", type.to_string(),
      "}"
    );
  }
#endif

  TypeCandidates(T* ast = nullptr)
    : ast(ast),
      is_deducted(false),
      specified_type(nullptr)
  {
  }
};

struct VariableDC : TypeCandidates<AST::VarDefine> {
  std::string_view name;

  bool is_argument{ };
  AST::Argument* ast_arg{ };

  // use std::map for prevent duplication
  std::map<AST::Variable*, bool> used_map;

#if METRO_DEBUG
  std::string to_string() const override;
#endif

  VariableDC()
    : is_argument(false),
      ast_arg(nullptr)
  {
  }
};

struct FunctionInfo {
  using FunctionDC = TypeCandidates<AST::Function>;

  std::string_view name;

  FunctionDC  dc;

  explicit FunctionInfo(AST::Function* func);

#if METRO_DEBUG
  std::string to_string() const;
#endif
};

struct ScopeInfo {
  AST::Scope* ast;

  std::vector<VariableDC> var_dc_list;

  VariableDC* find_var(std::string_view name) {
    for( auto&& dc : this->var_dc_list ) {
      if( dc.name == name )
        return &dc;
    }

    return nullptr;
  }

  VariableDC& append_var(std::string_view name) {
    auto& dc = this->var_dc_list.emplace_back();

    dc.name = name;

    return dc;
  }
};

class Sema {

  friend class TypeEvaluator;

  struct EvalResult {
    enum class Condition {
      Success,
      Incomplete,
    };

    ValueType type;

    Condition cond;

    bool fail() const {
      return this->cond != Condition::Success;
    }

    EvalResult(ValueType const& type = { })
      : type(type),
        cond(Condition::Success)
    {
    }

    EvalResult(Condition cond)
      : cond(cond)
    {
    }
  };

public:
  explicit Sema(AST::Scope* root);

  Sema(Sema&&) = delete;
  Sema(Sema const&) = delete;

  void analyze();
  void semantics_checker();

  ValueType* get_cache(AST::Base* ast);

  static Sema* get_instance();

private:

  void create_variable_dc();
  void deduction_variable_type(VariableDC& dc);

  void create_function_dc();
  void deduction_func_return_type(FunctionInfo& func);

  void check_semantics(AST::Base* ast);

  bool get_type_from_name(ValueType& out, std::string_view name);

  // TODO
  // StructInfo* find_struct(std::string_view name);

  FunctionInfo* find_func(std::string_view name);

  // VariableDC* get_variable_dc(AST::Variable* ast);
  std::tuple<VariableDC*, ScopeInfo*, size_t> get_variable_dc(AST::Variable* ast);

  ScopeInfo& get_cur_scope();

  // try evaluate a type of ast
  EvalResult try_eval_type(AST::Base* ast);

  ValueType eval_type(AST::Base* ast);

  ScopeInfo& enter_scope(AST::Scope* ast);
  void leave_scope(AST::Scope* ast);

  static bool is_lvalue(AST::Base* ast);

  static void get_last_expr(AST::Base* ast, ASTVector& out);
  static ASTVector get_returnable_expr(AST::Base* ast);

  //
  // create object from token
  static Object* create_obj(Token* token);

  static void ast_map(
    AST::Base* ast,
    std::function<void(AST::Base*)> fp,
    std::function<void(AST::Base*)> fp_begin = nullptr,
    std::function<void(AST::Base*)> fp_end = nullptr
  );

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


  AST::Scope* root;

  std::vector<FunctionInfo> functions;

  std::list<AST::Scope*> scope_history;
  std::map<AST::Scope*, ScopeInfo> scope_info_map;

  std::map<AST::Base*, ValueType> caches;

  bool deduction_updated{ };

  std::map<AST::Variable*, VariableDC*> var_dc_ptr_map;

  std::map<AST::VarDefine*, bool> var_assignmented_flag;
  
  std::map<AST::Value*, Object*> objects;

  void print_self();

};

} // namespace metro::semantics

} // namespace metro

