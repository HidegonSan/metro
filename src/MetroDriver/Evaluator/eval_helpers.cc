#include "MetroDriver/evaluator.h"
#include "MetroDriver/sema.h"
#include "GC.h"

namespace metro {

Object* Evaluator::construct_object_from_type(AST::Type* type) {
  auto obj = new Object;

  alert;
  obj->type = Sema::get_instance()->walk(type);

  alertios(obj->type.to_string());

  if( type->arr_depth != 0 ) {
    return obj;
  }

  if( !type->elems.empty() ) {
    for( auto&& elem : type->elems ) {
      obj->list.emplace_back(construct_object_from_type(elem));
    }
  }

  return obj;
}

Evaluator::ScopeInfo& Evaluator::enter_scope(AST::Scope* ast) {
  auto& ss = this->scope_stack.emplace_front(ast);

  this->cur_scope = &ss;

  return ss;
}

void Evaluator::leave_scope() {
  this->scope_stack.pop_front();

  if( this->scope_stack.empty() ) {
    this->cur_scope = nullptr;
    return;
  }

  this->cur_scope = &*this->scope_stack.begin();
}

} // namespace metro
