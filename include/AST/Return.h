#pragma once

namespace Metro::AST {
  struct Return : Base {
    AST::Base* expr;

    std::string to_string() const {
      return Utils::linkstr("<Return ", expr->to_string(), ">");
    }

    Return(Token* tok)
      : Base(AST::Kind::Return, tok)
    {
    }
  };
}