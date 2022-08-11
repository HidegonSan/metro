#pragma once

namespace Metro::AST {
  using namespace std::string_literals;

  struct Base {
    Kind    kind;
    Token*  token;
    bool    is_expr;

    bool is_empty_array() const;

    virtual std::string to_string() const;
    virtual SourceRange get_range_on_source() const;

  protected:
    explicit Base(Kind kind, Token* token)
      : kind(kind),
        token(token),
        is_expr(false)
    {
    }

    virtual ~Base() { }
  };
}