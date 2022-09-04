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

  alertphase("Sema: create dc of variables");
  this->create_variable_dc();

  alertphase("Sema: create dc of functions");
  this->create_function_dc();

  alertphase("Sema: deduction variable types");
  do {
    this->deduction_updated = false;

    for( auto&& [scope, info] : this->scope_info_map ) {
      for( auto&& var : info.var_dc_list )
        this->deduction_variable_type(var);
    }
  } while( this->deduction_updated );

  for( auto&& [scope, info] : this->scope_info_map ) {
    for( auto&& var : info.var_dc_list ) {
      if( !var.is_deducted ) {
         Error(ErrorKind::CannotInfer, var.ast, "cannot deduction the type of variable")
          .emit(true);
      }
    }
  }

  // deduction function return types
  alertphase("Sema: deduction the return types of functions");
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
