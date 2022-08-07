#include "AST.h"
#include "Sema/Sema.h"
#include "Error.h"
#include "Debug.h"
#include "Utils.h"

namespace Metro::Semantics {
  void Sema::analyze_func_return_type(ValueType& out, AST::Function* func) {
    std::vector<AST::Base*> lastexpr_list;

    // get last expressions
    get_lastval_full(lastexpr_list, func->code);

    // Analyze return-type
    if( func->return_type == nullptr ) {
      bool assignmented = false;

      for( auto&& last : lastexpr_list ){
        if( !contains_callfunc_in_expr(func->name, last) ) {
          auto&& tmp = walk(last);

          alertios("last-expr of " << func->name << ": '" << tmp.to_string() << "' from " << last->to_string());

          if( !assignmented ) {
            out = tmp;
            assignmented = true;
          }
          else if( !out.equals(tmp) ) {
            Error::add_error(ErrorKind::TypeMismatch, last,
              Utils::linkstr("expected '", out.to_string(), "', buf found '", tmp.to_string(), "'")
            );
          }
        }
      }

      if( !assignmented ) {
        Error::add_error(ErrorKind::CannotInfer, func->token, "failed to infer return type of function");
        Error::exit_app();
      }
    }
    else { // already specified
      out = walk(func->return_type);

      for( auto&& last : lastexpr_list ) {
        auto&& tmp = walk(last);

        if( !out.equals(tmp) ) {
          Error::add_error(ErrorKind::TypeMismatch, last,
            Utils::linkstr("expected '", out.to_string(), "', buf found '", tmp.to_string(), "'")
          );
        }
      }

      Error::check();
    }

    alertios("function " << func->name << ": return-type = " << out.to_string());
  }
}