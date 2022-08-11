#pragma once

// -----------------------
//  配列

//  Sema: 全ての要素が一貫して同じ型である必要がある
// -----------------------

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