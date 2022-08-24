// -----------------------
//  Type

//  型情報
// -----------------------
#pragma once

namespace metro::AST {
  struct Struct;
  struct Type : Base {
    std::string_view    name;
    std::vector<Type*>  elems;
    size_t arr_depth;
    bool  is_mutable;
    bool  is_reference;
    bool  have_elements;

    AST::Struct* userdef;

    std::string to_string() const;
    SourceRange get_range_on_source() const;

    Type(Token* tok)
      : Base(Kind::Type, tok),
        arr_depth(0),
        is_mutable(false),
        is_reference(false),
        have_elements(false),
        userdef(nullptr)
    {
    }
  };
}
