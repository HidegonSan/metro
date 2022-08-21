// ----------------------------- //
//  Lexer : 字句解析
// ----------------------------- //

#pragma once

#include <string>
#include "../source.h"

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

  size_t position;
  size_t linenum;

  std::string const& source;
  size_t const length;

  Source const& src;
};

}
