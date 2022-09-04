#pragma once

#include <list>
#include <map>
#include <vector>
#include <memory>
#include <stdexcept>
#include <concepts>
#include <functional>

namespace metro {

struct Token;
struct Object;
struct ValueType;

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

  bool is_deducted;
  AST::Type* specified_type;

  ValueType type;

  TypeCandidates(T* ast = nullptr)
    : ast(ast)
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

struct FunctionContext {
  using FunctionDC = TypeCandidates<AST::Function>;

  std::string_view name;

  FunctionDC  dc;

  FunctionContext(AST::Function* func)
    : name(func->name),
      dc(func)
  {
  }
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

  ValueType* get_cache(AST::Base* ast);

  static Sema* get_instance();

private:

  void create_variable_dc();
  void deduction_variable_type(VariableDC& dc);

  void create_function_dc();
  void deduction_func_return_type(FunctionContext& func);

  FunctionContext* find_func(std::string_view name);

  VariableDC* get_variable_dc(AST::Variable* ast);

  ScopeInfo& get_cur_scope();

  // try evaluate a type of ast
  EvalResult try_eval_type(AST::Base* ast);

  ValueType eval_type(AST::Base* ast);

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

  std::vector<FunctionContext> functions;

  std::list<AST::Scope*> scope_history;
  std::map<AST::Scope*, ScopeInfo> scope_info_map;

  std::map<AST::Base*, ValueType> caches;

  std::map<AST::Variable*, VariableDC*> var_dc_ptr_map;
  
};

} // namespace metro::semantics

} // namespace metro

