#include "AST.h"
#include "Error.h"
#include "MetroDriver/Parser.h"

namespace metro {

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
    Error(ErrorKind::InvalidSyntax, this->cur->prev,
      "expected '" + std::string(str) + "' after this token")
        .emit(true);
  }
}

void Parser::expect_ident() {
  if( cur->kind != TokenKind::Ident ) {
    Error(ErrorKind::InvalidSyntax, cur, "expected identifier")
      .emit(true);
  }
}

AST::Scope* Parser::expect_scope() {
  auto ast = new AST::Scope(cur);

  this->expect("{");

  if( this->eat("}") )
    return ast;

  do {
    auto item = this->expr();

    ast->append(item);

    auto&& prevstr = this->cur->prev->str;

    if( this->eat("}") ) {
      if( prevstr == ";" )
        ast->append(new AST::None(nullptr));
      
      break;
    }
    else if( prevstr != ";" && prevstr != "}" ) {
      this->expect_semi();
    }
  } while( this->check() );

  return ast;
}

AST::Type* Parser::expect_type() {
  auto ast = new AST::Type(this->cur);

  this->expect_ident();
  ast->name = this->cur->str;

  this->next();

  if( this->eat("<") ) {
    ast->have_elements = true;

    do {
      ast->elems.emplace_back(this->expect_type());
    } while( this->eat(",") );

    if( cur->str == ">>" ) {
      cur->str = ">";
      cur->insert(TokenKind::Punctuator, 1, ">");
    }

    this->expect(">");
  }

  while( this->eat("[]") )
    ast->arr_depth++;

  ast->is_mutable = this->eat("mut");
  ast->is_reference = this->eat("&");

  return ast;
}

void Parser::expect_semi() {
  expect(";");
}

} // namespace metro
