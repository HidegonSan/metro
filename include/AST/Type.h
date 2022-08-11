#pragma once

// -----------------------
//  Type

//  型情報
// -----------------------

namespace Metro::AST {
  struct Struct;
  struct Type : Base {
    std::string_view    name;
    std::vector<Type*>  elems;
    bool  is_constant;
    bool  is_reference;

    AST::Struct* userdef;

    std::string to_string() const;
    SourceRange get_range_on_source() const;

    Type(Token* tok)
      : Base(Kind::Type, tok),
        is_reference(false),
        is_constant(false),
        userdef(nullptr)
    {
    }
  };
}