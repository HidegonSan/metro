#pragma once

namespace Metro {
  struct Object;
}

namespace Metro::AST {
  struct Type;
  struct Let : Base {
    std::string_view  name;
    Type*             type;
    Base*             init;
    Object*           value;

    std::string to_string() const {
      auto&& ret = "<Let '" + std::string(name) + "'";

      if( type != nullptr ) {
        ret += " : " + type->to_string();
      }

      if( init != nullptr ) {
        ret += " = " + init->to_string();
      }

      return ret + ">";
    }

    Let(Token* token)
      : Base(Kind::Let, token),
        type(nullptr),
        init(nullptr),
        value(nullptr)
    {
    }
  };
}