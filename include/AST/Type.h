#pragma once

namespace Metro::AST {
  struct Struct;
  struct Type : Base {
    std::string_view    name;
    std::vector<Type*>  elems;
    bool  is_reference;
    bool  is_constant;

    AST::Struct* userdef;

    std::string to_string() const {
      return "<Type>";
    }

    Type(Token* tok)
      : Base(Kind::Type, tok),
        is_reference(false),
        is_constant(false),
        userdef(nullptr)
    {
    }
  };
}