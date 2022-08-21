#include "AST.h"
#include "MetroDriver/sema.h"
#include "MetroDriver/evaluator.h"
#include "Error.h"
#include "Debug.h"
#include "Utils.h"

namespace metro {

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
