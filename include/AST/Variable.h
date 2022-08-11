// -----------------------
//  Variable : 変数
// -----------------------
#pragma once

namespace Metro::AST {
  struct Variable : Base {
    std::string_view  name;
    Base*             defined; // <deprecated>

    std::string to_string() const;

    Variable(Token* tok)
      : Base(Kind::Variable, tok),
        defined(nullptr)
    {
    }
  };
}