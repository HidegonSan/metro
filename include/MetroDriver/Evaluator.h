// ----------------------------- //
//  Evaluator : 構文木を評価 (実行) する
// ----------------------------- //

#pragma once

#include <list>
#include <map>
#include <vector>

// #include "AST.h"

namespace metro {

namespace AST {  // forward declare

enum class Kind : int;

struct Base;
struct Scope;

}  // namespace AST

struct Token;
struct Object;

class Evaluator {
  using ASTKind = AST::Kind;

  struct ScopeInfo {
    AST::Scope* ast;
    std::vector<Object*> variables;

    explicit ScopeInfo(AST::Scope* ast) : ast(ast) {}

    ~ScopeInfo();
  };

 public:
  Evaluator();
  ~Evaluator();

  Evaluator(Evaluator&&) = delete;
  Evaluator(Evaluator const&) = delete;

  Object* eval(AST::Base*);

  Object*& construct_value(Token*);

 private:
  std::map<Token*, Object*> _constructed_value_obj;

  std::list<ScopeInfo> _scope_list;

  ScopeInfo& enter_scope(AST::Scope* ast);
  void leave_scope();

  ScopeInfo& get_cur_scope();

#if METRO_DEBUG
  struct _dbg_t {
    AST::Base* cur_ast;
  };

  _dbg_t _dbg;
#endif
};

}  // namespace metro
