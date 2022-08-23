#include "Types.h"
#include "MetroDriver/parser.h"
#include "Error.h"

namespace metro {

AST::Base* Parser::atom() {
  if( eat("true") ) {
    return new AST::Boolean(ate, true);
  }

  if( eat("false") ) {
    return new AST::Boolean(ate, false);
  }

  if( eat("none") ) {
    return new AST::None(ate);
  }

  switch( cur->kind ) {
    case TokenKind::Int:
    case TokenKind::Float:
    case TokenKind::Char:
    case TokenKind::String:
      next();
      return new AST::Value(cur->prev);

    case TokenKind::Ident:
      next();
      return new AST::Variable(cur->prev);
  }

  return nullptr;
}

} // namespace metro
