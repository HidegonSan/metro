// ----------------------------- //
//  Lexer : 字句解析
// ----------------------------- //

#pragma once

#include <string>

namespace metro {
  struct Token;
  class Lexer {
  public:
    explicit Lexer(Source const& src);

    Token* lex();

  private:
    bool check();
    char peek();
    bool match(std::string_view const& s);
    void pass_space();

    std::string const& source;
    size_t position;
    size_t const length;
    size_t linenum;
  };
}
