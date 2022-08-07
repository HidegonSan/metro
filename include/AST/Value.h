#pragma once

namespace Metro {
  struct Object;
}

namespace Metro::AST {
  struct Value : Base {
    Object*   object;
    
    std::string to_string() const {
      return "<Value '" + std::string(token->str) + "' >";
    }

    Value(Token* tok)
      : Base(Kind::Value, tok)
    {
    }
  };
}