#pragma once

namespace Metro::AST {
  struct Member;
  struct Struct : Base {
    std::string_view name;
    std::vector<Member> members;

    Struct(Token* tok)
      : Base(Kind::Struct, tok)
    {
    }
  };
}