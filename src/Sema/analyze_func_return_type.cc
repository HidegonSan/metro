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

          if( !assignmented ) {
            out = tmp;
            assignmented= 1;
          }
          else if( !out.equals(tmp) ) {
            Error::add_error(ErrorKind::TypeMismatch, last, "type mismatch 0fh3glk1");
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
        if( !out.equals(walk(last)) ) {
          Error::add_error(ErrorKind::TypeMismatch, last, "type mismatch 0b91nxd0");
        }
      }

      Error::check();
    }

    alertios("function " << func->name << ": return-type = " << out.to_string());
  }
}