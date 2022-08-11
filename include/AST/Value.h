// -----------------------
//  Value :  即値

//  オブジェクトの実態は Sema で作成＆割り当てられる
// -----------------------
#pragma once

namespace Metro::AST {
  struct Value : Base {
    Object*   object;

    std::string to_string() const;

    Value(Token* tok)
      : Base(Kind::Value, tok)
    {
    }
  };
}