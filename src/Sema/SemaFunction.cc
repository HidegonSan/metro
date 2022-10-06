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

  alert;

  if( func.dc.is_deducted )
    return;

  alert;

  if( func.dc.specified_type ) {
    alert;

    func.dc.type = this->eval_type(func.dc.specified_type);
    func.dc.is_deducted = true;
    return;
  }

  alert;

  alertios("function '" << func.name << "'");

  for( auto&& ast : func.dc.candidates ) {
    auto& dc = func.dc;
    auto&& res = this->try_eval_type(ast);

    alertios("tdc of func '" << func.name << "': " << ast->to_string());

    if( res.fail() )
      continue;

    if( dc.is_deducted ) {
      alertios(dc.type);
      alertios(res.type);

      if( !dc.type.equals(res.type) ) {
        Error(ErrorKind::TypeMismatch, ast,
          Utils::linkstr("expected '", dc.type.to_string(),
            "' but found '", res.type.to_string(), "'"))
              .emit();
      }
    }
    else {
      alertios("res.type = " << res.type);

      dc.type = res.type;
      dc.is_deducted = true;

      this->deduction_updated = true;
    }
  }



}


} // namespace metro::semantics

