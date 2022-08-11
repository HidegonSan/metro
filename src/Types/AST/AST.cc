#include "AST.h"
#include "Utils.h"

namespace Metro::AST {
  bool Base::is_empty_array() const {
    return kind == Kind::Array && ((AST::Array*)this)->elements.empty();
  }

  bool If::closed_with_else() const {
    auto x = if_false;

    while( x && x->kind == Kind::If ) {
      x = ((AST::If*)x)->if_false;
    }

    return x != nullptr;
  }

}