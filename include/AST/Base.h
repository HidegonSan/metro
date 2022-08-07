#pragma once

#include "Utils.h"

namespace Metro::AST {
  struct Base {
    Kind    kind;
    Token*  token;
    //bool    is_retval = false;
    bool    is_expr = false;

    virtual std::pair<size_t, size_t> get_range_on_source() {
      return { token->pos, token->pos + token->str.length() };
    }

    virtual std::string to_string() const {
      return Utils::format("<AST::Base at %p>", this);
    }

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