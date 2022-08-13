#include "Types.h"
#include "MetroDriver/Evaluator.h"
#include "GC.h"

namespace Metro {
  Object* Evaluator::construct_from_type(AST::Type* type) {
    ValueType valtype;

    if( type->name == "int" ) {
      valtype.kind = ValueType::Kind::Int;
    }
    else {
      valtype.kind = ValueType::Kind::UserDef;
    }

    if( type->is_constant ) {
      valtype.attr |= ValueType::ATTR_CONST;
    }

    if( type->is_reference ) {
      valtype.attr |= ValueType::ATTR_REFERENCE;
    }

    auto ret = gcnew();

    ret->type = valtype;

    return ret;
  }
}