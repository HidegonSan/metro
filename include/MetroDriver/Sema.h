#pragma once

#include <list>
#include <map>
#include <vector>
#include <stdexcept>
#include <concepts>
#include <functional>

namespace metro {

struct ValueType;

namespace AST {

struct Base;
struct Variable;
struct VarDefine;
struct Scope;
struct Function;

} // namespace metro::AST

namespace semantics {

using ASTKind = AST::Kind;
using ASTVector = std::vector<AST::Base*>;
using TypeVector = std::vector<ValueType>;

template <std::derived_from<AST::Base> T>
struct TypeCandidates {
  T* ast;
  std::vector<AST::Base*> candidates;

  bool is_deducted;
  AST::Type* specified_type;

  ValueType result;

  TypeCandidates()
    : ast(nullptr)
  {
  }
};

struct VariableDC : TypeCandidates<AST::VarDefine> {
  bool is_argument;

  std::string_view name;

  AST::Argument* ast_arg;

  VariableDC()
    : is_argument(false),
      ast_arg(nullptr)
  {
  }
};

struct FunctionDC : TypeCandidates<AST::Function> {
  std::string_view name;

  FunctionDC() { }
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

  struct EvaluatedResult {
    enum class Condition {
      Completed,
      Incomplete,
      NullPointer,
    };

    AST::Base* ast;

    union {
      Condition cond;
      ErrorKind error;
    };

    ValueType type;

    EvaluatedResult(Condition cond = Condition::Completed)
      : ast(nullptr),
        cond(cond)
    {
    }
  };

public:
  explicit Sema(AST::Scope* root);

  Sema(Sema&&) = delete;
  Sema(Sema const&) = delete;

  void analyze();

  ValueType* get_cache(AST::Base* ast);

  static Sema* get_instance();

private:

  // for ast_map()
  void mapfn_begin() {

  }

  // for ast_map()
  void mapfn_end() {

  }

  void create_variable_dc();
  void deduction_variable_types();

  void create_function_dc();
  void deduction_func_return_type(FunctionDC& func);


  VariableDC* get_variable_dc(AST::Variable* ast);

  ScopeInfo& get_cur_scope();

  ValueType eval_type(AST::Base* ast);
  EvaluatedResult try_eval_type(AST::Base* ast);

  ScopeInfo& enter_scope(AST::Scope* ast);
  void leave_scope();

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

  std::vector<FunctionDC> functions;

  std::list<AST::Scope*> scope_history;
  std::map<AST::Scope*, ScopeInfo> scope_info_map;

  std::map<AST::Base*, ValueType> caches;

  std::map<AST::Variable*, VariableDC*> var_dc_ptr_map;
  
};

} // namespace metro::semantics

} // namespace metro

