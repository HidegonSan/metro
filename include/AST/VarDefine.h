#pragma once

namespace Metro {
  struct Object;
}

namespace Metro::AST {
  struct Type;
  struct VarDefine : Base {
    std::string_view  name;
    Type*             type;
    Base*             init;
    Object*           value;

    std::string to_string() const;

    VarDefine(Token* token)
      : Base(Kind::VarDefine, token),
        type(nullptr),
        init(nullptr),
        value(nullptr)
    {
    }
  };
}