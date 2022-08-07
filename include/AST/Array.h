#pragma once

namespace Metro::AST {
  struct Array : Base {
    std::vector<AST::Base*> elements;

    Array(Token* tok)
      : Base(Kind::Array, tok)
    {
    }
  };
}