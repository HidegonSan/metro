#include "AST.h"
#include "Error.h"
#include "Utils.h"
#include "MetroDriver/Sema.h"

namespace metro::semantics {

static Sema* _inst;

Sema::Sema(AST::Scope* root)
  : root(root)
{
  _inst = this;
}

void Sema::analyze() {

  alert;
  this->create_variable_dc();

  alert;
  this->create_function_dc();

  // deduction variable types
  for( auto&& [scope, info] : this->scope_info_map ) {
    for( auto&& var : info.var_dc_list )
      this->deduction_variable_type(var);
  }

  // deduction function return types
  for( auto&& func : this->functions ) {
    this->deduction_func_return_type(func);
  }

}

ValueType* Sema::get_cache(AST::Base* ast) {
  if( this->caches.contains(ast) )
    return &this->caches[ast];

  return nullptr;
}

Sema* Sema::get_instance() {
  return _inst;
}

} // namespace metro::semantics
