#pragma once

namespace Metro::AST {
  using namespace std::string_literals;

  struct Base {
    Kind    kind;
    Token*  token;
    bool    is_expr = false;

    bool is_empty_array() const;

    virtual std::pair<size_t, size_t> get_range_on_source() {
      return { token->pos, token->pos + token->str.length() };
    }

    virtual std::string to_string() const = 0;

  protected:
    Base()
      : kind(Kind::None),
        token(nullptr)
    {
    }

    Base(Kind kind, Token* token)
      : kind(kind),
        token(token)
    {
    }

    virtual ~Base() { }
  };
}