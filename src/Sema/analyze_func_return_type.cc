#include "AST.h"
#include "Sema/Sema.h"
#include "Error.h"
#include "Debug.h"
#include "Utils.h"

namespace Metro::Semantics {
  ValueType Sema::analyze_func_return_type(AST::Function* func) {

    std::vector<AST::Base*> lastexpr_list;

    cfn_ast = func;

    // return type
    auto rettype = walk(func->return_type);
    bool have_rettype_ast = func->return_type != nullptr;

    // get last expressions
    get_lastval_full(lastexpr_list, func->code);

    // Analyze return-type
    if( !have_rettype_ast ) {
      bool assignmented = false;

      for( auto&& last : lastexpr_list ){
        if( !contains_callfunc_in_expr(func->name, last) ) {
          auto tmp = walk(last);
          
          if( !assignmented ) {
            rettype = tmp;
            assignmented= 1;
          }
          else if( !rettype.equals(tmp) ) {
            Error::add_error(ErrorKind::TypeMismatch, last, "type mismatch 0fh3glk1");
          }
        }
      }

      if( !assignmented ) {
        Error::add_error(ErrorKind::CannotInfer, func->token, "failed to infer return type of function");
        Error::exit_app();
      }
    }
    else{ // already specified
      for( auto&& last : lastexpr_list ){
        if( !rettype.equals(walk(last)) ) {
          Error::add_error(ErrorKind::TypeMismatch, last, "type mismatch 0b91nxd0");
        }
      }

      Error::check();
    }

    alertios("function " << func->name << ": return-type = " << rettype.to_string());

    return rettype;
  }
}