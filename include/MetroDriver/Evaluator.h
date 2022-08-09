#pragma once

namespace Metro {
  struct Object;
  namespace AST {
    struct Base;
  }

  class Evaluator {
  public:
    Evaluator() { }

    // =====
    //  Evaluate tree
    // ==================
    Object* eval(AST::Base* ast);
    Object** eval_lvalue(AST::Base* ast);

    //
    // 演算子を評価する
    Object* eval_operator(AST::Kind kind, Object* left, Object* right);

    //
    // 型情報を元にオブジェクトを構築する
    Object* construct_from_type(AST::Type* type);

  };
}