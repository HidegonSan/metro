#pragma once

namespace Metro::AST {
  struct Return : Base {
    AST::Base* expr;

    Return(Token* tok)
      : Base(AST::Kind::Return, tok)
    {
    }
  };
}