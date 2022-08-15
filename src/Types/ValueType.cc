#include "Types.h"
#include "Debug.h"
#include "Utils.h"

namespace Metro {
  bool ValueType::is_no_extents() const {
    return
      !is_mutable &&
      !is_reference;
  }

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

      case Kind::Tuple:
        ret = "tuple";
        break;

      case Kind::None:
        ret = "none";
        break;

      default: {
        TODO_IMPL
      }
    }

    if( !elems.empty() ) {
      ret += "<" + Utils::join<ValueType>(", ", elems) + ">";
    }

    for( size_t i = 0; i < arr_depth; i++ ) {
      ret += "[]";
    }

    if( is_mutable ) {
      ret += " mut";
    }

    if( is_reference ) {
      ret += "&";
    }

    return ret;
  }

  std::vector<std::pair<char const*, ValueType::Kind>> const ValueType::name_table {
    { "int", Kind::Int },
    { "float", Kind::Float },
    { "char", Kind::Char },
    { "string", Kind::String },
    { "tuple", Kind::Tuple },
    { "none", Kind::None },
  };

}