// ----------------------------- //
//  Evaluator : 構文木を評価 (実行) する
// ----------------------------- //

#pragma once

#include <list>
#include "AST.h"

namespace metro {

class Evaluator {

  struct ScopeInfo {
    AST::Scope* ast;
    bool jump_to_end; // loop expression

    std::vector<Object*> variables;

    auto& push_var(Object* obj) {
      return this->variables.emplace_back(obj);
    }

    explicit ScopeInfo(AST::Scope* ast)
      : ast(ast),
        jump_to_end(false)
    {
    }
  };

  struct CallStack {
    AST::Function* func;
    bool is_returned;

    std::vector<Object*> arguments;

    explicit CallStack(AST::Function* func)
      : func(func),
        is_returned(false)
    {
    }
  };

public:
  Evaluator();

  //
  // evaluate
  Object* eval(AST::Base* ast);
  Object** eval_lvalue(AST::Base* ast);

  //
  // operators
  Object* eval_operator(AST::Kind kind, Object* left, Object* right);

  //
  // compareing operators
  Object* compute_compare(AST::Compare* ast);

  //
  // 型情報を元にオブジェクトを構築する
  Object* construct_object_from_type(AST::Type* type);

private:

  // ScopeInfo& cur_scope_stack() {
  //   return *this->scope_stack.begin();
  // }

  // CallStack& cur_call_stack() {
  //   return *this->call_stack.begin();
  // }

  ScopeInfo& enter_scope(AST::Scope* ast);
  void leave_scope();

  CallStack& enter_func(AST::Function* ast);
  void leave_func();

  //size_t call_depth;

  ScopeInfo* cur_scope;
  CallStack* cur_call_stack;

  // std::vector<Object*> global_var;

  std::list<ScopeInfo> scope_stack;
  std::list<CallStack> call_stack;

  
};

} // namespace metro
