#pragma once

namespace Metro::AST {
  struct Boolean : Base {
    bool const val;
    
    std::string to_string() const {
      return val ? "true" : "false";
    }

    Boolean(Token* tok)
      : Base(Kind::Boolean, tok),
        val(tok->str == "true")
    {
    }
  };
}