// -----------------------
//  スコープ

//  last-expr は全て同じ型である必要がある
// -----------------------
#pragma once

namespace metro::AST {
  struct Scope : ListBase {
    std::string to_string() const;
    SourceRange get_range_on_source() const;

    auto& append(Base* ast) {
      return elements.emplace_back(ast);
    }

    Scope(Token* tok)
      : ListBase(Kind::Scope, tok)
    {
    }
  };
}
