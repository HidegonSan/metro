#pragma once

namespace Metro::AST {
  struct Expr : Base {
    Base*   lhs;
    Base*   rhs;

    std::string to_string() const {
      return "<Expr " + lhs->to_string() + " "
        + std::string(token->str) + " " + rhs->to_string() + ">";
    }

    explicit Expr(Kind kind, Base* lhs, Base* rhs, Token* tok)
      : lhs(lhs),
        rhs(rhs)
    {
      this->kind = kind;
      this->token = tok;
    }
  };
}