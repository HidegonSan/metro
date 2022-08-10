#pragma once

namespace Metro::AST {
  struct Array : Base {
    std::vector<AST::Base*> elements;

    std::string to_string() const {
      std::string ret { "<Array " };

      for( int64_t i = 0; i < elements.size(); i++ ) {
        ret += std::to_string(i) + ": " + elements[i]->to_string();
        if( i < elements.size() - 1 ) ret += ", ";
      }

      return ret + ">";
    }

    Array(Token* tok)
      : Base(Kind::Array, tok)
    {
    }
  };
}