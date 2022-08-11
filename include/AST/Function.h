// -----------------------
//  ユーザー定義関数

//  戻り値の指定がない場合は、Sema::analyze_func_return_type() で推論される
// -----------------------
#pragma once

namespace Metro::AST {
  struct Function : Base {
    std::string_view        name;
    std::vector<Argument>   args;
    Type*                   return_type; // after '->' token
    Scope*                  code;

    std::string to_string() const;
    SourceRange get_range_on_source() const;

    Function(Token* token)
      : Base(Kind::Function, token),
        return_type(nullptr),
        code(nullptr)
    {
    }
  };
}