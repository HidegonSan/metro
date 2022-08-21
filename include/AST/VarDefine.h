// -----------------------
//  VarDefine : 変数定義

//  let 文で使われます
// -----------------------
#pragma once

namespace metro::AST {
  struct Type;
  struct VarDefine : Base {
    std::string_view  name;
    Type*             type;
    Base*             init;
    Object*           value;

    std::string to_string() const;
    SourceRange get_range_on_source() const;

    VarDefine(Token* token)
      : Base(Kind::VarDefine, token),
        type(nullptr),
        init(nullptr),
        value(nullptr)
    {
    }
  };
}
