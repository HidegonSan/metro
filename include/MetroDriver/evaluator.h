// ----------------------------- //
//  Evaluator : 構文木を評価 (実行) する
// ----------------------------- //

#pragma once

namespace metro {
  class Evaluator {
  public:
    Evaluator() { }

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
  };
}
