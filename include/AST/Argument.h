#pragma once

namespace Metro::AST {
  struct Argument : Base {
    std::string_view    name;
    Type*               type;
    Object*             value;

    std::string to_string() const;
    SourceRange get_range_on_source() const;

    Argument(Token* tok)
      : Base(Kind::Argument, tok),
        type(nullptr),
        value(nullptr)
    {
    }
  };
}