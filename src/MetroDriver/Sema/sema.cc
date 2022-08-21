#include "AST.h"
#include "MetroDriver/Sema.h"
#include "MetroDriver/Evaluator.h"
#include "Error.h"
#include "Debug.h"
#include "Utils.h"

namespace metro::Semantics {
  static Sema* __inst;

  Sema::Sema(AST::Scope* root) {
    __inst = this;

    this->root = root;

    // make functions
    for( auto&& x : root->elems ) {
      if( x->kind == ASTKind::Function ) {
        functions.emplace_back((AST::Function*)x);
      }
    }
  }

  Sema* Sema::get_instance() {
    return __inst;
  }
}
