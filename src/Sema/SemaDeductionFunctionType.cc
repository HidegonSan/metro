#include "AST.h"
#include "Error.h"
#include "Utils.h"
#include "MetroDriver/Sema.h"

namespace metro::semantics {

void Sema::create_function_dc() {

  for( auto&& elem : this->root->elements ) {
    if( elem->kind == ASTKind::Function ) {
      auto&& ast = (AST::Function*)elem;

      auto& func = this->functions.emplace_back();

      func.ast = ast;
      func.name = ast->name;

      if( ast->return_type ) {
        func.is_deducted = true;
        func.specified_type = ast->return_type;
      }

      func.candidates = this->get_returnable_expr(ast->code);
    }
  }

}

void Sema::deduction_func_return_type(FunctionDC& func) {

  TypeVector tmp;

  for( auto&& ast : func.candidates ) {
    auto&& result = this->try_eval_type(ast);


  }



}


} // namespace metro::semantics

