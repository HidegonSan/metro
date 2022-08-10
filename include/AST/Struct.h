#pragma once

namespace Metro::AST {
  struct Struct : Base {
    struct Member {
      std::string_view name;
      Type* type;
    };

    std::string_view name;
    std::vector<Member> members;

    std::string to_string() const {
      return Utils::linkstr(
        "<Struct '", name, "' {",
        Utils::join(", ", members, [] (auto&& x) {
          return Utils::linkstr(x.name, ": ", x.type->to_string());
        }),
        "}>"
      );
    }

    Struct(Token* tok)
      : Base(Kind::Struct, tok)
    {
    }
  };
}