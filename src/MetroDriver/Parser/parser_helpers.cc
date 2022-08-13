#include "Types.h"
#include "MetroDriver/Parser.h"
#include "Error.h"

namespace Metro {
  bool Parser::check() {
    return cur && cur->kind != TokenKind::End;
  }

  void Parser::next() {
    cur = cur->next;
  }

  bool Parser::eat(std::string_view str) {
    if( check() && cur->str == str ) {
      ate = cur;
      next();
      return true;
    }

    return false;
  }

  void Parser::expect(std::string_view str) {
    if( !eat(str) ) {
      Error::add_error(ErrorKind::ExpectedToken, cur, "expected '" + std::string(str) + "' before this token");
      Error::exit_app();
    }
  }

  void Parser::expect_ident() {
    if( cur->kind != TokenKind::Ident ) {
      Error::add_error(ErrorKind::ExpectedToken, cur, "expected identifier");
      Error::exit_app();
    }
  }

  AST::Scope* Parser::expect_scope() {
    auto ast = new AST::Scope(cur);

    expect("{");

    if( eat("}") ) {
      return ast;
    }

    while( check() ) {
      auto item = expr();

      ast->append(item);

      if( eat(";") ) {
        if( eat("}") ) {
          ast->append(new AST::None(nullptr));
          break;
        }
      }
      else if( !eat("}") ) {
        if( is_need_semi(item) ) {
          expect_semi();
        }
      }
      else {
        break;
      }
    }

    return ast;
  }

  AST::Type* Parser::expect_type() {
    auto ast = new AST::Type(cur);

    expect_ident();
    ast->name = cur->str;

    next();

    if( eat("<") ) {
      do {
        ast->elems.emplace_back(expect_type());
      } while( eat(",") );

      if( cur->str == ">>" ) {
        cur->str = ">";
        cur->insert(TokenKind::Punctuator, 1, ">");
      }

      expect(">");
    }

    while( eat("[]") ) {
      ast->arr_depth++;
    }

    ast->is_constant = eat("const");
    ast->is_reference = eat("&");

    return ast;
  }

  bool Parser::is_need_semi(AST::Base* ast) {
    using AST::Kind;

    switch( ast->kind ) {
      case Kind::If:
      case Kind::Function:
      case Kind::Struct:
      case Kind::Scope:
        return false;
    }

    return true;
  }

  void Parser::expect_semi() {
    expect(";");
  }
}