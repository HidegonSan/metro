// -----------------------
//  None
// -----------------------
#pragma once

namespace metro::AST {
  struct None : Base {
    std::string to_string() const;

    None(Token* tok)
      : Base(Kind::None, tok)
    {
    }
  };
}
