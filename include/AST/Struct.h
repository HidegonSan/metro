#pragma once

namespace Metro::AST {
  struct Member;
  struct Struct : Base {
    std::string_view name;
    std::vector<Member> members;

    std::string to_string() const {
      return "<Struct>";
    }

    Struct(Token* tok)
      : Base(Kind::Struct, tok)
    {
    }
  };
}