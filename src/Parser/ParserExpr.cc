#include "AST.h"
#include "Error.h"
#include "MetroDriver/Parser.h"

namespace metro {

AST::Base* Parser::subscript() {
  auto x = this->factor();

  while (this->check() && this->eat("[")) {
    auto tok = ate;
    x = new AST::Expr(AST::Kind::Subscript, x, this->expr(), tok);
  }

  return x;
}

AST::Base* Parser::member() {
  auto x = this->subscript();

  while (this->check() && this->eat(".")) {
    x = new AST::Expr(AST::Kind::MemberAccess, x, this->factor(), this->ate);
  }

  return x;
}

AST::Base* Parser::mul() {
  auto x = member();

  while (check()) {
    if (eat("*"))
      x = new AST::Expr(AST::Kind::Mul, x, member(), ate);
    else if (eat("/"))
      x = new AST::Expr(AST::Kind::Div, x, member(), ate);
    else
      break;
  }

  return x;
}

AST::Base* Parser::add() {
  auto x = mul();

  while (check()) {
    auto tok = cur;

    if (eat("+"))
      x = new AST::Expr(AST::Kind::Add, x, mul(), tok);
    else if (eat("-"))
      x = new AST::Expr(AST::Kind::Sub, x, mul(), tok);
    else
      break;
  }

  return x;
}

AST::Base* Parser::shift() {
  auto x = this->add();

  while (this->check()) {
    auto tok = this->cur;

    if (this->eat("<<"))
      x = new AST::Expr(AST::Kind::LShift, x, this->add(), tok);
    else if (this->eat(">>"))
      x = new AST::Expr(AST::Kind::RShift, x, this->add(), tok);
    else
      break;
  }

  return x;
}

AST::Base* Parser::compare() {
  using ItemKind = AST::Compare::Item::Kind;

  auto x = this->add();

  while (this->check()) {
    auto tok = this->cur;

    if (this->eat(">"))
      AST::Compare::create(x)->append(ItemKind::BiggerLeft, tok, this->add());
    else if (this->eat("<"))
      AST::Compare::create(x)->append(ItemKind::BiggerRight, tok, this->add());
    else if (this->eat(">="))
      AST::Compare::create(x)->append(ItemKind::BiggerOrEqualLeft, tok,
                                      this->add());
    else if (this->eat("<="))
      AST::Compare::create(x)->append(ItemKind::BiggerOrEqualRight, tok,
                                      this->add());
    else if (this->eat("=="))
      AST::Compare::create(x)->append(ItemKind::Equal, tok, this->add());
    else if (this->eat("!="))
      AST::Compare::create(x)->append(ItemKind::NotEqual, tok, this->add());
    else
      break;
  }

  return x;
}

AST::Base* Parser::assign() {
  auto x = compare();

  if (eat("=")) {
    x = new AST::Expr(AST::Kind::Assign, x, assign(), ate);
  }

  return x;
}

AST::Base* Parser::expr() { return assign(); }

}  // namespace metro
