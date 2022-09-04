#include "AST.h"
#include "Error.h"
#include "Utils.h"
#include "MetroDriver/Sema.h"

namespace metro::semantics {

void Sema::create_function_dc() {

  for( auto&& elem : this->root->elements ) {
    if( elem->kind == ASTKind::Function ) {
      auto&& ast = (AST::Function*)elem;

      auto& ctx = this->functions.emplace_back(ast);

      auto& dc = ctx.dc;

      if( ast->return_type ) {
        dc.is_deducted = true;
        dc.specified_type = ast->return_type;
      }

      dc.candidates = this->get_returnable_expr(ast->code);
    }
  }

}

void Sema::deduction_func_return_type(FunctionContext& func) {

  std::vector<EvalResult> tmp;

  auto& dc = func.dc;

  alertios("function '" << func.name << "'");

  for( auto&& ast : dc.candidates ) {
    auto& result = tmp.emplace_back(this->try_eval_type(ast));

    
  }



}


} // namespace metro::semantics

