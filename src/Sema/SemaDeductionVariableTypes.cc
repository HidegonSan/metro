#include "AST.h"
#include "Error.h"
#include "Utils.h"
#include "MetroDriver/Sema.h"

namespace metro::semantics {

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

void Sema::deduction_variable_types() {

  this->create_variable_dc();

  for( auto&& pair : this->scope_info_map ) {
    auto&& [scope, info] = pair;

    for( auto&& dc : info.var_dc_list ) {
      if( dc.is_argument )
        continue;
      
      
    }

    debug(
      printf("%p\n", scope);

      for( auto&& dc : info.var_dc_list ) {
        auto&& name = dc.name;

        if( dc.is_argument ) {
          std::cout << "arg: " << name << std::endl;
        }
        else {
          std::cout
            << name << std::endl
            << dc.ast->to_string() << std::endl;
        }

        for( auto&& x : dc.candidates ) {
          std::cout << x->to_string() << std::endl;
        }
      }
    )
  }

}

VariableDC* Sema::get_variable_dc(AST::Variable* ast) {
  for( auto&& scope : this->scope_history )
    if( auto dc = this->scope_info_map[scope].find_var(ast->name); dc )
      return dc;

  return nullptr;
}

} // namespace metro::semantics

