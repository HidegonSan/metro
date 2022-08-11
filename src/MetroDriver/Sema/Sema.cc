#include "AST.h"
#include "MetroDriver/Sema.h"
#include "MetroDriver/Evaluator.h"
#include "Error.h"
#include "Debug.h"
#include "Utils.h"

namespace Metro::Semantics {
  Sema::Sema(AST::Scope* root) {
    this->root = root;

    // make functions
    for( auto&& x : root->elems ) {
      if( x->kind == ASTKind::Function ) {
        functions.emplace_back((AST::Function*)x);
      }
    }
  }
}