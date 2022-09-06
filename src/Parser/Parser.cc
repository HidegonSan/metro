#include "AST.h"
#include "Error.h"
#include "MetroDriver/Parser.h"

namespace metro {

Parser::Parser(Token* token)
  : cur(token),
    ate(nullptr)
{
}

AST::Base* Parser::parse() {
  auto scope = new AST::Scope(nullptr);

  while( this->check() ) {
    auto x = scope->append(this->toplevel());

    if( this->eat(";") )
      continue;
    else if( this->check() && this->cur->prev->str != ";" && this->cur->prev->str != "}" )
      this->expect_semi();
  }

  return scope;
}

} // namespace metro
