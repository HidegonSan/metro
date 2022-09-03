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
}

void Sema::analyze() {
  for( auto&& x : root->elements ) {
    if( x->kind == ASTKind::Function ) {
      functions.emplace_back((AST::Function*)x);
    }
  }

  this->scopelist.emplace_front().ast = root;

  for( auto&& ast : this->root->elements ) {
    this->walk(ast);
  }
}

Sema* Sema::get_instance() {
  return __inst;
}

} // namespace metro
