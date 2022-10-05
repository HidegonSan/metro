#include "MetroDriver/Evaluator.h"

#include "AST.h"
#include "MetroDriver/Sema.h"

namespace metro {

Evaluator::Evaluator() {}
Evaluator::~Evaluator() {}

Object* Evaluator::eval(AST::Base* ast) {
  if (!ast) return nullptr;

  switch (ast->kind) {
    case ASTKind::None:
      break;

    case ASTKind::VarDefine: {
      auto x = (AST::VarDefine*)ast;
      auto& cur_scope = this->get_cur_scope();

      auto& pobj = cur_scope.variables.emplace_back();

      if (x->init) {
        pobj = this->eval(x->init);
      } else if (x->type) {
        pobj = new Object(*semantics::Sema::get_instance()->get_cache(x->type));
      }

      break;
    }

    case ASTKind::Value: {
      alert;
      return this->construct_value(ast->token);
    }

    case ASTKind::Variable: {
      auto x = (AST::Variable*)ast;

      alertios(x->index);

      auto& scope = this->get_cur_scope();

      return scope.variables[x->index];
    }

    case ASTKind::Scope: {
      auto x = (AST::Scope*)ast;

      if (x->elements.empty()) break;

      Object* last;

      this->enter_scope(x);

      for (auto&& elem : x->elements) {
        last = this->eval(elem);
      }

      this->leave_scope();

      return last;
    }

    default:
      alertios("kind = " << static_cast<int>(ast->kind));
      TODO_IMPL;
  }

  return Object::none;
}

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