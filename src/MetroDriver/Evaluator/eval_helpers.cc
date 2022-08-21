#include "Types.h"
#include "MetroDriver/Evaluator.h"
#include "MetroDriver/Sema.h"
#include "GC.h"

namespace metro {
  Object* Evaluator::construct_from_type(AST::Type* type) {
    auto obj = new Object;

    alert;
    obj->type = Semantics::Sema::get_instance()->walk(type);

    alertios(obj->type.to_string());

    if( type->arr_depth != 0 ) {
      return obj;
    }

    if( !type->elems.empty() ) {
      for( auto&& elem : type->elems ) {
        obj->list.emplace_back(construct_from_type(elem));
      }
    }

    return obj;
  }
}
