#pragma once

namespace Metro::AST {
  struct For : Base {
    Base* init;
    Base* cond;
    Base* counter;
    Base* code;

    std::string to_string() const;
    SourceRange get_range_on_source() const;

    For(Token* tok)
      : Base(Kind::For, tok),
        init(nullptr),
        cond(nullptr),
        counter(nullptr),
        code(nullptr)
    {
    }
  };
}