#pragma once

namespace Metro::AST {
  struct Struct : Base {
    struct Member {
      std::string_view name;
      Type* type;
      Token* token;
    };

    std::string_view name;
    std::vector<Member> members;

    std::string to_string() const;

    Struct(Token* tok)
      : Base(Kind::Struct, tok)
    {
    }
  };
}