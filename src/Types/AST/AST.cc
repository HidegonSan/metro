#include "AST.h"

namespace Metro::AST {
  bool Base::is_empty_array() const {
    return kind == Kind::Array && ((AST::Array*)this)->elements.empty();
  }
}