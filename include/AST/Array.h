#pragma once

namespace Metro::AST {
  struct Array : Base {
    std::vector<AST::Base*> elements;

    std::string to_string() const;
    SourceRange get_range_on_source() const;

    Array(Token* tok)
      : Base(Kind::Array, tok)
    {
    }
  };
}