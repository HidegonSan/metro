#pragma once

namespace Metro::AST {
  struct Variable : Base {
    std::string_view  name;
    Base*             defined;

    std::string to_string() const {
      return "<Variable>";
    }

    Variable(Token* tok)
      : Base(Kind::Variable, tok),
        defined(nullptr)
    {
    }
  };
}