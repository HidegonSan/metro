#pragma once

namespace Metro::AST {
  struct Member : Base {
    std::string_view name;
    Type* type;

    std::string to_string() const {
      return "<Member>";
    }

    Member(Token* tok)
      : Base(Kind::Member, tok)
    {
    }
  };
}