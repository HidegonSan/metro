#include "MetroDriver/Evaluator.h"

#include "AST.h"
#include "GC.h"
#include "MetroDriver/Sema.h"

namespace metro {

Evaluator::ScopeInfo::~ScopeInfo() {
  // todo: reduce ref_count in varibales
}

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

    default: {
      if (!ast->is_expr) {
        alertios("kind = " << static_cast<int>(ast->kind));
        TODO_IMPL;
      }

      auto x = (AST::Expr*)ast;

      auto lhs = this->eval(x->lhs);
      auto rhs = this->eval(x->rhs);

      auto const& typekind = lhs->type.kind;

      switch (ast->kind) {
        case ASTKind::Add: {
          if (lhs->type.arr_depth) {
            lhs->list.emplace_back(rhs);
            break;
          }

          switch (typekind) {
            case ValueType::Kind::Int:
              ((MeLong*)lhs)->v_long += ((MeLong*)rhs)->v_long;
              break;
          }

          break;
        }
      }

      return lhs;
    }
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