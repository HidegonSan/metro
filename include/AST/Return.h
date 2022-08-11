#pragma once

namespace Metro::AST {
  struct Return : Base {
    AST::Base* expr;

    std::string to_string() const;

    Return(Token* tok)
      : Base(AST::Kind::Return, tok),
        expr(nullptr)
    {
    }
  };
}