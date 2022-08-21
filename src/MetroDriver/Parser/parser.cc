#include "Types.h"
#include "MetroDriver/parser.h"
#include "Error.h"

namespace metro {

Parser::Parser(Token* token)
  : cur(token),
    ate(nullptr)
{
}

AST::Base* Parser::parse() {
  auto scope = new AST::Scope(nullptr);

  while( check() ) {
    auto x = scope->append(toplevel());

    if( is_need_semi(x) && cur->kind != TokenKind::End ) {
      expect_semi();
    }
  }

  return scope;
}

} // namespace metro
