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

AST::Base* Parser::factor() {

  // bracket
  if( this->eat("(") ) {
    auto tok = ate;
    auto x = this->expr();

    // tuple
    if( eat(",") ) {
      x = new AST::Tuple(tok, x);

      do {
        ((AST::Tuple*)x)->elements.emplace_back(this->expr());
      } while( eat(",") );
    }

    this->expect(")");
    return x;
  }

  // controls
  if( auto ast = this->controls(); ast )
    return ast;

  // scope
  if( cur->str == "{" )
    return expect_scope();

  // array
  if( eat("[") ) {
    auto ast = new AST::Array(ate);

    if( !eat("]") ) {
      do {
        ast->elements.emplace_back(expr());
      } while( eat(",") );

      expect("]");
    }

    return ast;
  }

  // atom
  if( auto _atom = this->atom(); _atom != nullptr ) {
    if( _atom->kind == AST::Kind::Variable && this->eat("(") ) {
      auto call = new AST::CallFunc(_atom->token);

      call->name = ((AST::Variable*)_atom)->name;

      if( !this->eat(")") ) {
        do {
          call->args.emplace_back(expr());
        } while( this->eat(",") );

        this->expect(")");
      }

      return call;
    }

    return _atom;
  }

  Error(ErrorKind::InvalidSyntax, cur, "invalid syntax")
    .emit(true);
}

AST::Base* Parser::subscript() {
  auto x = this->factor();

  while( this->check() && this->eat("[") ) {
    auto tok = ate;
    x = new AST::Expr(AST::Kind::Subscript, x, this->expr(), tok);
  }

  return x;
}

AST::Base* Parser::member() {
  auto x = this->subscript();

  while( this->check() && this->eat(".") ) {
    x = new AST::Expr(AST::Kind::MemberAccess, x, this->factor(), this->ate);
  }

  return x;
}

AST::Base* Parser::mul() {
  auto x = member();

  while( check() ) {
    if( eat("*") ) x = new AST::Expr(AST::Kind::Mul, x, member(), ate);
    else if( eat("/") ) x = new AST::Expr(AST::Kind::Div, x, member(), ate);
    else break;
  }

  return x;
}

AST::Base* Parser::add() {
  auto x = mul();

  while( check() ) {
    auto tok = cur;

    if( eat("+") ) x = new AST::Expr(AST::Kind::Add, x, mul(), tok);
    else if( eat("-") ) x = new AST::Expr(AST::Kind::Sub, x, mul(), tok);
    else break;
  }

  return x;
}

AST::Base* Parser::compare() {
  using ItemKind = AST::Compare::Item::Kind;

  auto x = this->add();

  while( this->check() ) {
    auto tok = this->cur;

    if( this->eat(">") ) AST::Compare::create(x)->append(ItemKind::BiggerLeft, tok, this->add());
    else if( this->eat("<") ) AST::Compare::create(x)->append(ItemKind::BiggerRight, tok, this->add());
    else if( this->eat(">=") ) AST::Compare::create(x)->append(ItemKind::BiggerOrEqualLeft, tok, this->add());
    else if( this->eat("<=") ) AST::Compare::create(x)->append(ItemKind::BiggerOrEqualRight, tok, this->add());
    else if( this->eat("==") ) AST::Compare::create(x)->append(ItemKind::Equal, tok, this->add());
    else if( this->eat("!=") ) AST::Compare::create(x)->append(ItemKind::NotEqual, tok, this->add());
    else break;
  }

  return x;
}

AST::Base* Parser::assign() {
  auto x = compare();

  if( eat("=") ) {
    x = new AST::Expr(AST::Kind::Assign, x, assign(), ate);
  }

  return x;
}

AST::Base* Parser::expr() {
  return assign();
}

} // namespace metro
