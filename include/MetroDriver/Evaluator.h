// ----------------------------- //
//  Evaluator : 構文木を評価 (実行) する
// ----------------------------- //

#pragma once

namespace metro {
  class Evaluator {
  public:
    Evaluator() { }

    //
    //  Evaluate AST
    Object* eval(AST::Base* ast);
    Object** eval_lvalue(AST::Base* ast);

    //
    // 演算子
    Object* eval_operator(AST::Kind kind, Object* left, Object* right);

    //
    // 型情報を元にオブジェクトを構築する
    Object* construct_from_type(AST::Type* type);
  };
}
