// -----------------------
//  Type

//  型情報
// -----------------------
#pragma once

namespace Metro::AST {
  struct Struct;
  struct Type : Base {
    std::string_view    name;
    std::vector<Type*>  elems;
    size_t arr_depth;
    bool  is_constant;
    bool  is_reference;

    AST::Struct* userdef;

    std::string to_string() const;
    SourceRange get_range_on_source() const;

    Type(Token* tok)
      : Base(Kind::Type, tok),
        arr_depth(0),
        is_constant(false),
        is_reference(false),
        userdef(nullptr)
    {
    }
  };
}