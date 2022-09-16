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
        dc.type = this->eval_type(ast->return_type);
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
      alert;

      dc.type = res.type;
      dc.is_deducted = true;
      return;
    }

    crash;
  }

  alert;

  alertios("function '" << func.name << "'");

  for( auto&& ast : dc.candidates ) {
    auto&& res = this->try_eval_type(ast);

    alertios("tdc of func '" << func.name << "': " << ast->to_string());

    if( res.fail() )
      continue;

    if( dc.is_deducted ) {
      if( !dc.type.equals(res.type) ) {
        Error(ErrorKind::TypeMismatch, ast,
          Utils::linkstr("expected '", dc.type.to_string(),
            "' but found '", res.type.to_string(), "'"))
              .emit();
      }
    }
    else {
      dc.type = res.type;
      dc.is_deducted = true;

      this->deduction_updated = true;
    }
  }



}


} // namespace metro::semantics

