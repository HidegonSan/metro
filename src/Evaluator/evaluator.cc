#include "Evaluator.h"

#include "AST.h"

namespace metro {

Evaluator::Evaluator() {}
Evaluator::~Evaluator() {}

Object* Evaluator::eval(AST::Base* ast) {
  switch (ast->kind) {
    case ASTKind::Value: {
      return this->_constructed_value_obj[ast->token];
    }
  }
}

Object*& Evaluator::construct_value(Token* token) {
  auto& ret = this->_constructed_value_obj[token];

  if (ret != nullptr) return ret;

  switch (token->kind) {
    case TokenKind::Int: {
      ret = new MeLong(std::atoi(token->str.data()));
      break;
    }

    default:
      TODO_IMPL
  }

#if METRO_DEBUG
  assert(ret != nullptr);
#endif

  return ret;
}

}  // namespace metro