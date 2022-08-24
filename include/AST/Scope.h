// -----------------------
//  スコープ

//  last-expr は全て同じ型である必要がある
// -----------------------
#pragma once

namespace metro::AST {
  struct Scope : Base {
    std::vector<Base*>  elems;

    std::string to_string() const;
    SourceRange get_range_on_source() const;

    auto& append(Base* ast) {
      return elems.emplace_back(ast);
    }

    Scope(Token* tok)
      : Base(Kind::Scope, tok)
    {
    }
  };
}
