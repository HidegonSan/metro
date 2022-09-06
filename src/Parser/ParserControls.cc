#include "AST.h"
#include "Error.h"
#include "MetroDriver/Parser.h"

namespace metro {

AST::Base* Parser::controls() {
  //
  // if
  if( this->eat("if") ) {
    auto ast = new AST::If(ate);

    ast->cond = this->expr();
    ast->if_true = this->expect_scope();

    if( this->eat("else") ) {
      if( cur->str == "if" )
        ast->if_false = this->expr();
      else
        ast->if_false = this->expect_scope();
    }

    return ast;
  }

  //
  // for
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

    this->expect_semi();

    return ast;
  }

  if( this->eat("return") ) {
    if( auto tok = this->ate; this->eat(";") )
      return new AST::Return(tok);

    auto ast = new AST::Return(this->ate);

    ast->expr = this->expr();

    this->expect_semi();

    return ast;
  }

  return nullptr;
}

} // namespace metro
