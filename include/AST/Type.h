#pragma once

namespace Metro::AST {
  struct Struct;
  struct Type : Base {
    std::string_view    name;
    std::vector<Type*>  elems;
    bool  is_constant;
    bool  is_reference;

    AST::Struct* userdef;

    std::string to_string() const {
      auto&& ret = "<Type '" + std::string(name) + "'";

      if( !elems.empty() ) {
        ret += " elems{" + Utils::join(", ", elems, [] (auto x) { return x->to_string(); }) + "}";
      }

      if( is_constant ) {
        ret += " const";
      }

      if( is_reference ) {
        ret += "&";
      }

      return ret + ">";
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