#pragma once

namespace Metro::AST {
  struct Return : Base {
    AST::Base* expr;

    std::string to_string() const;
    SourceRange get_range_on_source() const;

    Return(Token* tok)
      : Base(AST::Kind::Return, tok),
        expr(nullptr)
    {
    }
  };
}