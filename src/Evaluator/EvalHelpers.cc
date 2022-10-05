#include "AST.h"
#include "GC.h"
#include "MetroDriver/Evaluator.h"

namespace metro {

Object*& Evaluator::construct_value(Token* token) {
  alert;

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

  alert;

  return ret;
}

Evaluator::ScopeInfo& Evaluator::enter_scope(AST::Scope* ast) {
  return this->_scope_list.emplace_front(ast);
}

void Evaluator::leave_scope() { this->_scope_list.pop_front(); }

Evaluator::ScopeInfo& Evaluator::get_cur_scope() {
  return *this->_scope_list.begin();
}

}  // namespace metro
