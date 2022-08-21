// -----------------------
//  if 式

//  成立時処理、不成立時処理はどちらも同じ型である必要がある
// -----------------------
#pragma once

namespace metro::AST {
  struct If : Base {
    Base*   cond;
    Base*   if_true;
    Base*   if_false;

    bool closed_with_else() const;

    std::string to_string() const;
    SourceRange get_range_on_source() const;

    If(Token* tok)
      : Base(Kind::If, tok),
        cond(nullptr),
        if_true(nullptr),
        if_false(nullptr)
    {
    }
  };
}
