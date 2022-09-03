#pragma once

#include <list>
#include <map>
#include <vector>

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

struct VariableDC {
  bool is_argument;

  std::string_view name;

  AST::VarDefine* ast;
  AST::Argument* ast_arg;

  bool is_deducted;
  ValueType var_type;

  AST::Type* specified_type;

  std::vector<AST::Base*> candidates;

  VariableDC()
    : is_argument(false),
      ast(nullptr),
      ast_arg(nullptr),
      is_deducted(false),
      specified_type(nullptr)
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

public:
  explicit Sema(AST::Scope* root)
    : root(root)
  {
  }

  Sema(Sema&&) = delete;
  Sema(Sema const&) = delete;

  void analyze();

private:

  void create_variable_dc();

  void deduction_variable_types();

  VariableDC* get_variable_dc(AST::Variable* ast);

  ScopeInfo& get_cur_scope();

  ValueType eval_type(AST::Base* ast);

  ScopeInfo& enter_scope(AST::Scope* ast);
  void leave_scope();

  static void get_last_expr(AST::Base* ast, ASTVector& out);
  static ASTVector get_returnable_expr(AST::Base* ast);

  //
  // create object from token
  static Object* create_obj(Token* token);


  AST::Scope* root;

  std::list<AST::Scope*> scope_history;
  std::map<AST::Scope*, ScopeInfo> scope_info_map;

  std::map<AST::Base*, ValueType> caches;

  std::map<AST::Variable*, VariableDC*> var_dc_ptr_map;
  
};

void ast_map(
  AST::Base* ast,
  std::function<void(AST::Base*)> fp,
  std::function<void(AST::Base*)> fp_begin = nullptr,
  std::function<void(AST::Base*)> fp_end = nullptr
);

} // namespace metro::semantics

} // namespace metro

