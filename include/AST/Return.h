// -----------------------
//  return 式

//  戻り値の記述がない場合は AST::
// -----------------------
#pragma once

namespace metro::AST {
  struct Return : Base {
    AST::Base* expr; // for return value

    std::string to_string() const;
    SourceRange get_range_on_source() const;

    Return(Token* tok)
      : Base(AST::Kind::Return, tok),
        expr(nullptr)
    {
    }
  };
}
