#pragma once

namespace Metro {
  struct Object;
  namespace AST {
    struct Base;
  }

  class Evaluator {
  public:
    Evaluator() { }

    Object* eval(AST::Base* ast);
    Object** eval_lvalue(AST::Base* ast);

    Object* construct_from_type(AST::Type* type);

  };
}