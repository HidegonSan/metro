#include "AST.h"
#include "Error.h"
#include "MetroDriver/Parser.h"

namespace metro {

AST::Base* Parser::atom() {
  if (eat("true")) return new AST::Boolean(ate, true);

  if (eat("false")) return new AST::Boolean(ate, false);

  if (eat("none")) return new AST::None(ate);

  switch (cur->kind) {
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
  if (this->eat("(")) {
    auto tok = ate;
    auto x = this->expr();

    // tuple
    if (eat(",")) {
      x = new AST::Tuple(tok, x);

      do {
        ((AST::Tuple*)x)->elements.emplace_back(this->expr());
      } while (eat(","));
    }

    this->expect(")");
    return x;
  }

  // scope
  if (cur->str == "{") return expect_scope();

  // array
  if (eat("[")) {
    auto ast = new AST::Array(ate);

    if (!eat("]")) {
      do {
        ast->elements.emplace_back(expr());
      } while (eat(","));

      expect("]");
    }

    return ast;
  }

  // variable declaration
  if (eat("let")) {
    auto ast = new AST::VarDefine(ate);

    expect_ident();
    ast->name = cur->str;

    next();

    if (eat(":")) {
      ast->type = expect_type();
    }

    if (eat("=")) {
      ast->init = expr();
    }

    this->expect_semi();

    return ast;
  }

  // controls
  if (auto ast = this->controls(); ast) return ast;

  // atom
  if (auto _atom = this->atom(); _atom != nullptr) {
    if (_atom->kind == AST::Kind::Variable && this->eat("(")) {
      auto call = new AST::CallFunc(_atom->token);

      call->name = ((AST::Variable*)_atom)->name;

      if (!this->eat(")")) {
        do {
          call->args.emplace_back(expr());
        } while (this->eat(","));

        this->expect(")");
      }

      return call;
    }

    return _atom;
  }

  Error(ErrorKind::InvalidSyntax, cur, "invalid syntax").emit(true);
}

}  // namespace metro
