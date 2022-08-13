#include "Types.h"
#include "MetroDriver/Evaluator.h"
#include "MetroDriver/Sema.h"
#include "GC.h"

namespace Metro {
  Object* Evaluator::construct_from_type(AST::Type* type) {
    auto obj = new Object;

    obj->type = Semantics::Sema::get_instance()->walk(type);

    if( !type->elems.empty() ) {
      for( auto&& elem : type->elems ) {
        obj->list.emplace_back(construct_from_type(elem));
      }
    }

    return obj;
  }
}