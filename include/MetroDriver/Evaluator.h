// ----------------------------- //
//  Evaluator : 構文木を評価 (実行) する
// ----------------------------- //

#pragma once

#include <list>
#include <map>

// #include "AST.h"

namespace metro {

namespace AST {  // forward declare
struct Base;
}

struct Token;
struct Object;

class Evaluator {
  using ASTKind = AST::Kind;

 public:
  Evaluator();
  ~Evaluator();

  Evaluator(Evaluator&&) = delete;
  Evaluator(Evaluator const&) = delete;

  Object* eval(AST::Base*);

  Object*& construct_value(Token*);

 private:
  std::map<Token*, Object*> _constructed_value_obj;

#if METRO_DEBUG
  struct _dbg_t {
    AST::Base* cur_ast;
  };

  _dbg_t _dbg;
#endif
};

}  // namespace metro
