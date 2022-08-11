#pragma once

namespace Metro::AST {
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