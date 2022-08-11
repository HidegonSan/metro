#pragma once

namespace Metro::AST {
  struct Array : Base {
    std::vector<AST::Base*> elements;

    std::string to_string() const;

    Array(Token* tok)
      : Base(Kind::Array, tok)
    {
    }
  };
}