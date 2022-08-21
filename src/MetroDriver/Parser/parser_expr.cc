#include "Types.h"
#include "MetroDriver/parser.h"
#include "Error.h"

namespace metro {

AST::Base* Parser::factor() {

  auto stmt_ast = stmt();

  if( stmt_ast != nullptr ) {
    return stmt_ast;
  }

  if( cur->str == "{" ) {
    return expect_scope();
  }

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

    case TokenKind::Ident: {
      if( cur->next->str == "(" ) {
        auto ast = new AST::CallFunc(cur);

        ast->name = cur->str;
        next();

        next();

        if( !eat(")") ) {
          do {
            ast->args.emplace_back(expr());
          } while( eat(",") );

          expect(")");
        }

        return ast;
      }

      auto ast = new AST::Variable(cur);
      ast->name = cur->str;

      next();
      return ast;
    }
  }

  // Error::add_error(ErrorKind::InvalidSyntax, cur, "invalid syntax");
  // Error::exit_app();

  Error(ErrorKind::InvalidSyntax, cur, "invalid syntax")
    .emit(true);
}

AST::Base* Parser::member() {
  auto x = factor();

  while( check() && eat(".") ) {
    x = new AST::Expr(AST::Kind::MemberAccess, x, factor(), ate);
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

  auto x = add();

  while( check() ) {
    if( eat(">") ) AST::Compare::create(x)->append(ItemKind::BiggerLeft, ate, add());
    else if( eat("<") ) AST::Compare::create(x)->append(ItemKind::BiggerRight, ate, add());
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

}
