// -----------------------
//  Variable : 変数
// -----------------------
#pragma once

namespace metro::AST {
  struct Variable : Base {
    std::string_view  name;
    
    bool is_arg;

    std::string to_string() const;

    Variable(Token* tok)
      : Base(Kind::Variable, tok),
        name(tok->str),
        is_arg(false)
    {
    }
  };
}
