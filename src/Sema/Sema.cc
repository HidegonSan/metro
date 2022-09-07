#include "AST.h"
#include "Error.h"
#include "Utils.h"
#include "MetroDriver/Sema.h"

namespace metro::semantics {

static Sema* _inst;

FunctionInfo::FunctionInfo(AST::Function* func)
  : name(func->name),
    dc(func)
{
}

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

    for( auto&& [scope, info] : this->scope_info_map )
      for( auto&& var : info.var_dc_list )
        this->deduction_variable_type(var);
  } while( this->deduction_updated );

  // final
  for( auto&& [scope, info] : this->scope_info_map )
    for( auto&& var : info.var_dc_list )
      if( var.used_map.empty() )
        Error(ErrorKind::UnusedVariable, var.ast, "unused variable")
          .set_warn()
          .emit();
      else if( !var.is_deducted )
          Error(ErrorKind::CannotInfer, var.ast, "cannot deduction the type of variable")
            .emit(true);

  // deduction function return types
  alertphase("Sema: deduction the return types of functions");
  do {
    this->deduction_updated = false;

    for( auto&& func : this->functions )
      this->deduction_func_return_type(func);
  } while( this->deduction_updated );

  for( auto&& func : this->functions )
    if( !func.dc.is_deducted )
      Error(ErrorKind::CannotInfer, func.dc.ast->token->next,
        "cannot deduction the type of function '" + std::string(func.name) + "'")
        .emit(true);

  debug(
    for( auto&& func : this->functions ) {
      alertios("function '" << func.name << "': return=" << func.dc.type.to_string());
    }
  )

}

void Sema::semantics_checker() {
  this->enter_scope(root);

  for( auto&& func : this->functions ) {
    this->check_semantics(func.dc.ast);
  }

  this->leave_scope(root);
}

ValueType* Sema::get_cache(AST::Base* ast) {
  if( this->caches.contains(ast) )
    return &this->caches[ast];

  return nullptr;
}

Sema* Sema::get_instance() {
  return _inst;
}

void Sema::check_semantics(AST::Base* ast) {
  switch( ast->kind ) {
    case ASTKind::Array:
    case ASTKind::Tuple: {
      auto x = (AST::ListBase*)ast;


      break;
    }

    case ASTKind::Variable: {
      auto x = (AST::Variable*)ast;

      auto dc = this->var_dc_ptr_map[x];

      auto is_assignmented = this->var_assignmented_flag[dc->ast];

      if( !is_assignmented ) {
        Error(ErrorKind::UninitializedValue, x, "used uninitialized variable")
          .emit();
      }

      break;
    }

    case ASTKind::Assign: {
      auto x = (AST::Expr*)ast;

      auto&& dest = this->eval_type(x->lhs);

      if( !dest.is_mutable ) {
        Error(ErrorKind::NotMutable, x->token, "expected lvalue expression at left side")
          .emit();
      }
    }
  }
}

} // namespace metro::semantics
