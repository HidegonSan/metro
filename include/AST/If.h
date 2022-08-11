#pragma once

namespace Metro::AST {
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