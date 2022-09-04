#include "AST.h"
#include "Error.h"
#include "Utils.h"
#include "MetroDriver/Sema.h"

namespace metro::semantics {

VariableDC* Sema::get_variable_dc(AST::Variable* ast) {
  for( auto&& scope : this->scope_history )
    if( auto dc = this->scope_info_map[scope].find_var(ast->name); dc )
      return dc;

  return nullptr;
}

void Sema::create_variable_dc() {

  ast_map(
    this->root,
    [this] (AST::Base* ast) {
      switch( ast->kind ) {
        case ASTKind::VarDefine: {
          auto x = (AST::VarDefine*)ast;
          auto& scope = this->get_cur_scope();

          if( scope.find_var(x->name) ) {
            // shadowing
            TODO_IMPL
          }

          auto& dc = scope.append_var(x->name);

          dc.ast = x;

          if( x->type ) {
            dc.is_deducted = true;
            dc.specified_type = x->type;
          }

          if( x->init ) {
            dc.candidates.emplace_back(x->init);
          }

          break;
        }

        case ASTKind::Variable: {
          auto x = (AST::Variable*)ast;
          auto dc = this->get_variable_dc(x);

          if( !dc ) {
            Error(ErrorKind::UndefinedVariable, ast, "undefined variable name")
              .emit(true);
          }

          this->var_dc_ptr_map[x] = dc;

          break;
        }
        
      }
    },
    [this] (AST::Base* ast) {
      switch( ast->kind ) {
        case ASTKind::Scope:
          this->enter_scope((AST::Scope*)ast);
          break;

        case ASTKind::Function: {
          auto x = (AST::Function*)ast;
          auto& scope = this->scope_info_map[x->code];

          for( auto&& arg : x->args ) {
            auto&& dc = scope.append_var(arg.name);

            dc.is_argument = true;
            dc.ast_arg = &arg;
            dc.is_deducted = true;
            dc.specified_type = arg.type;
          }

          break;
        }
      }
    },
    [this] (AST::Base* ast) {
      switch( ast->kind ) {
        case ASTKind::Scope:
          this->leave_scope();
          break;

        case ASTKind::Assign: {
          auto x = (AST::Expr*)ast;

          if( x->lhs->kind == ASTKind::Variable ) {
            auto var = (AST::Variable*)x->lhs;
            auto dc = this->var_dc_ptr_map[var];

            assert(dc != nullptr);

            dc->candidates.emplace_back(x->rhs);
          }

          break;
        }
      }
    }
  );


}

void Sema::deduction_variable_type(VariableDC& dc) {

  if( dc.is_argument )
    return;

  if( dc.is_deducted && dc.specified_type ) {
    dc.type = this->eval_type(dc.ast->type);
    return;
  }

  for( auto&& c : dc.candidates ) {
    auto&& res = this->try_eval_type(c);

    if( !res.fail() ) {
      if( dc.is_deducted ) {
        if( !dc.type.equals(res.type) ) {
          Error(ErrorKind::TypeMismatch, c,
            Utils::linkstr("expected '", dc.type.to_string(),
              "' but found '", res.type.to_string(), "'"))
                .emit();
        }
      }
      else {
        dc.type = res.type;
        dc.is_deducted = true;
      }
    }
  }

  if( !dc.is_deducted ) {
    Error(ErrorKind::CannotInfer, dc.ast, "cannot deduction the type of variable")
      .emit(true);
  }

  alertios("variable '" << dc.name << "': " << dc.type);

}


} // namespace metro::semantics

