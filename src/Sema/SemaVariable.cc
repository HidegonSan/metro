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

  auto mapfn_var = [this] (AST::Variable* ast, bool allow_un) -> std::pair<bool, VariableDC*> {
    auto dc = this->get_variable_dc(ast);

    if( !dc ) {
      Error(ErrorKind::UndefinedVariable, ast, "undefined variable name")
        .emit(true);
      return { false, nullptr };
    }

    if( !allow_un && !dc->is_argument && dc->candidates.empty() ) {
      Error(ErrorKind::UninitializedValue, ast, "uninitialized variable")
        .emit(true);
      return { false, dc };
    }

    this->var_dc_ptr_map[ast] = dc;
    return { true, dc };
  };

  ast_map(
    this->root,
    [&] (AST::Base* ast) {
      switch( ast->kind ) {
        case ASTKind::VarDefine: {
          auto x = (AST::VarDefine*)ast;
          auto& scope = this->get_cur_scope();

          if( scope.find_var(x->name) ) {
            // todo: shadowing
            TODO_IMPL
          }

          auto& dc = scope.append_var(x->name);

          dc.ast = x;

          if( x->type ) {
            dc.specified_type = x->type;
          }

          if( x->init ) {
            dc.candidates.emplace_back(x->init);
          }

          break;
        }

        case ASTKind::Variable: {
          if( auto&& [x, y] = mapfn_var((AST::Variable*)ast, false); x && y )
            y->used_map[(AST::Variable*)ast] = true;

          break;
        }

        case ASTKind::Assign: {
          auto x = (AST::Expr*)ast;

          if( x->lhs->kind == ASTKind::Variable ) {
            auto var = (AST::Variable*)x->lhs;

            mapfn_var(var, true);

            auto dc = this->get_variable_dc(var);

            // assert(dc != nullptr);

            dc->candidates.emplace_back(x->rhs);
          }

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

          assert(x->code->kind == ASTKind::Scope);
          auto& scope = this->enter_scope(x->code);

          for( auto&& arg : x->args ) {
            auto&& dc = scope.append_var(arg.name);

            dc.is_argument = true;
            dc.ast_arg = &arg;
            dc.specified_type = arg.type;

            // dc.type = this->eval_type(arg.type);
            // dc.is_deducted = true;

            alertios(dc.name << " " << dc.type);
          }

          this->leave_scope(x->code);

          break;
        }
      }
    },
    [this] (AST::Base* ast) {
      switch( ast->kind ) {
        case ASTKind::Scope:
          this->leave_scope((AST::Scope*)ast);
          break;

      }
    }
  );


}

void Sema::deduction_variable_type(VariableDC& dc) {
  alert;

  alertios(dc.name);
  alertios(dc.to_string());

  if( dc.is_deducted )
    return;

  if( dc.specified_type ) {
    dc.type = this->eval_type(dc.specified_type);
    dc.is_deducted = true;
    return;
  }

  for( auto&& c : dc.candidates ) {
    alert;

    auto&& res = this->try_eval_type(c);

    if( !res.fail() ) {
      alert;

      if( dc.is_deducted ) {
        if( !dc.type.equals(res.type) ) {
          alert;

          Error(ErrorKind::TypeMismatch, c,
            Utils::linkstr("expected '", dc.type.to_string(),
              "' but found '", res.type.to_string(), "'"))
                .emit();
        }
      }
      else {
        alert;

        dc.type = res.type;
        dc.is_deducted = true;

        this->deduction_updated = true;
      }
    }
  }
}

} // namespace metro::semantics

