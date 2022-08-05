#include "Types.h"
#include "MetroDriver/Parser.h"
#include "Error.h"

namespace Metro {
  Parser::Parser(Token* token)
    : cur(token),
      ate(nullptr)
  {
  }

  AST::Base* Parser::parse() {
    return expr();

    // ----- //

    auto scope = new AST::Scope(nullptr);

    while( check() ) {
      auto x = scope->append(toplevel());

      if( is_need_semi(x) && cur->kind != TokenKind::End ) {
        expect_semi();
      }
    }

    return scope;
  }
}