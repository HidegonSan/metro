#pragma once

namespace Metro::AST {
  struct Expr : Base {
    Base*   lhs;
    Base*   rhs;

    std::string to_string() const;
    SourceRange get_range_on_source() const;

    explicit Expr(Kind kind, Base* lhs, Base* rhs, Token* tok)
      : Base(kind, tok),
        lhs(lhs),
        rhs(rhs)
    {
      this->kind = kind;
      this->token = tok;
      this->is_expr = true;
    }
  };
}