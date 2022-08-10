#pragma once

namespace Metro::AST {
  struct Function : Base {
    std::string_view        name;
    std::vector<Argument>   args;
    Type*                   return_type;
    Scope*                  code;

    std::string to_string() const {
      return Utils::linkstr(
        "<Function '", name, "' (", Utils::join(", ", args), ") ",
        return_type != nullptr ? ("-> " + return_type->to_string()) : "", " ", code->to_string(), ">"
      );
    }

    Function(Token* token)
      : Base(Kind::Function, token),
        return_type(nullptr),
        code(nullptr)
    {
    }
  };
}