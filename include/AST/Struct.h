#pragma once

// -----------------------
//  構造体
// -----------------------

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
    SourceRange get_range_on_source() const;

    Struct(Token* tok)
      : Base(Kind::Struct, tok)
    {
    }
  };
}