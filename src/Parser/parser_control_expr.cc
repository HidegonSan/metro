#include "Types.h"
#include "MetroDriver/parser.h"
#include "Error.h"

namespace metro {

AST::Base* Parser::stmt() {
  if( eat("if") ) {
    auto ast = new AST::If(ate);

    ast->cond = expr();
    ast->if_true = expect_scope();

    if( eat("else") ) {
      if( cur->str == "if" ) {
        ast->if_false = expr();
      }
      else {
        ast->if_false = expect_scope();
      }
    }

    return ast;
  }

  if( eat("for") ) {
    auto ast = new AST::For(ate);

    if( !eat(";") ) {
      ast->init = expr();
      expect(";");
    }

    if( !eat(";") ) {
      ast->cond = expr();
      expect(";");
    }

    if( cur->str != "{" ) {
      ast->counter = expr();
    }

    ast->code = expect_scope();

    return ast;
  }

  if( eat("let") ) {
    auto ast = new AST::VarDefine(ate);

    expect_ident();
    ast->name = cur->str;

    next();

    if( eat(":") ) {
      ast->type = expect_type();
    }

    if( eat("=") ) {
      ast->init = expr();
    }

    return ast;
  }

  if( this->eat("return") ) {
    if( auto tok = this->ate; this->eat(";") )
      return new AST::Return(tok);

    auto ast = new AST::Return(this->ate);

    ast->expr = this->expr();

    this->expect(";");
    return ast;
  }

  return nullptr;
}

} // namespace metro
