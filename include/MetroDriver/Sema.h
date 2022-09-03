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
  AST::VarDefine* ast;
  std::vector<AST::Base*> candidates;

  bool is_deducted;
  ValueType deducted_type;

  VariableDC(AST::VarDefine* ast = nullptr)
    : ast(ast),
      is_deducted(false)
  {
  }
};

struct ScopeInfo {
  AST::Scope* ast;

  std::map<std::string_view, VariableDC> var_dc_map;
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


  static ASTVector get_returnable_expr(AST::Base* ast);

  //
  // create object from token
  static Object* create_obj(Token* token);


  AST::Scope* root;

  std::list<AST::Scope*> scope_history;
  std::map<AST::Scope*, ScopeInfo> scope_info_map;

  std::map<AST::Base*, ValueType> caches;
  
};

} // namespace metro::semantics

} // namespace metro

