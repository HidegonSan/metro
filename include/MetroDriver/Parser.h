// ----------------------------- //
//  Parser : 構文解析
// ----------------------------- //

#pragma once

// #include "AST.h"

namespace metro {

struct Token;
struct Object;

namespace AST {

struct Base;
struct Scope;
struct Type;

}

class Parser {
public:
  explicit Parser(Token* token);

  AST::Base* atom();
  AST::Base* factor();

  AST::Base* subscript();
  AST::Base* member();

  AST::Base* mul();
  AST::Base* add();
  AST::Base* compare();
  AST::Base* assign();

  AST::Base* expr();
  AST::Base* controls();

  AST::Base* toplevel();

  AST::Base* parse();

private:
  bool check();
  void next();
  bool eat(std::string_view str);

  void expect(std::string_view str);
  void expect_ident();
  AST::Scope* expect_scope();
  AST::Type* expect_type();

  void expect_semi();

  Token*  cur;
  Token*  ate;
};

} // namespace metro

