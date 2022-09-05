#include "AST.h"
#include "Error.h"
#include "Utils.h"
#include "MetroDriver/Sema.h"

namespace metro::semantics {

void Sema::create_function_dc() {

  for( auto&& elem : this->root->elements ) {
    alert;

    if( elem->kind == ASTKind::Function ) {
      auto&& ast = (AST::Function*)elem;

      auto& ctx = this->functions.emplace_back(ast);

      auto& dc = ctx.dc;

      if( ast->return_type ) {
        alert;
        dc.is_deducted = true;
        dc.specified_type = ast->return_type;
      }

      dc.candidates = this->get_returnable_expr(ast->code);
    }
  }

}

void Sema::deduction_func_return_type(FunctionInfo& func) {

  std::vector<EvalResult> tmp;

  auto& dc = func.dc;

  alert;

  if( dc.is_deducted )
    return;

  alert;

  if( dc.specified_type ) {
    auto&& res = this->try_eval_type(dc.specified_type);

    if( !res.fail() ) {
      dc.type = res.type;
      dc.is_deducted = true;
      return;
    }
    else {
      TODO_IMPL
    }
  }

  alert;

  alertios("function '" << func.name << "'");

  for( auto&& ast : dc.candidates ) {
    auto&& result = this->try_eval_type(ast);

    alertios(ast->to_string());

    if( result.fail() )
      continue;

    if( dc.is_deducted ) {
      if( !dc.type.equals(result.type) ) {
        Error(ErrorKind::TypeMismatch, ast,
          Utils::linkstr("expected '", dc.type.to_string(),
            "' but found '", result.type.to_string(), "'"))
              .emit();
      }
    }
    else {
      dc.type = result.type;
      dc.is_deducted = true;

      this->deduction_updated = true;
    }
  }



}


} // namespace metro::semantics

