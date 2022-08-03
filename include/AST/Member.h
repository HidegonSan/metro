#pragma once

namespace Metro::AST {
  struct Member : Base {
    std::string_view name;
    Type* type;

    Member(Token* tok)
      : Base(Kind::Member, tok)
    {
    }
  };
}