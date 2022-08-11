#pragma once

// -----------------------
//  Boolean
// -----------------------

namespace Metro::AST {
  struct Boolean : Base {
    bool const val;

    std::string to_string() const;

    Boolean(Token* tok, bool val)
      : Base(Kind::Boolean, tok),
        val(val)
    {
    }
  };
}