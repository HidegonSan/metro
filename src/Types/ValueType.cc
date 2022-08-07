#include "Types.h"
#include "Debug.h"

namespace Metro {
  bool ValueType::equals(ValueType::Kind kind) const {
    return this->kind == kind;
  }

  bool ValueType::equals(ValueType const& type) const {
    if( kind != type.kind )
      return false;
    
    return true;
  }

  std::string ValueType::to_string() const {
    auto ret = std::string{ };

    switch( kind ) {
      case Kind::Int:
        ret = "int";
        break;

      case Kind::String:
        ret = "string";
        break;

      case Kind::None:
        ret = "none";
        break;

      default: {
        TODO_IMPL
      }
    }

    if( attr & ATTR_CONST ) {
      ret += " const";
    }

    if( attr & ATTR_REFERENCE ) {
      ret += "&";
    }

    return ret;
  }
}