#include "Application.h"
#include "Types/Object.h"
#include "Utils.h"
#include "Debug.h"

namespace metro {

Object* Object::none = new Object(ValueType::Kind::None);

Object::Object(ValueType type)
  : type(type),
    ref_count(0),
    is_weak(false),
    v_int(0)
{
debug(
  alert;
  alertctor(Object);
)
}

Object::~Object() {
debug(
  alert;
  fprintf(stderr, "\t#Object destructing: %p\n", this);
)
}

//
// object to string
std::string Object::to_string() const {
  static bool str_q = false;

  if( type.arr_depth != 0 ) {
    return "[" + Utils::join(", ", list, [] (auto& p) { return p->to_string(); }) + "]";
  }

  switch( type.kind ) {
    case ValueType::Kind::Int:
      return std::to_string(v_int);

    case ValueType::Kind::Float: {
      auto&& ret = std::to_string(v_float);

      while( *ret.rbegin() == '0' ) {
        ret.pop_back();
      }

      return ret;
    }

    case ValueType::Kind::Bool:
      return v_bool ? "true" : "false";

    case ValueType::Kind::Char:
      return Utils::Strings::to_string(std::u16string(v_char, 1));

    case ValueType::Kind::String: {
      if( str_q )
        return '"' + Utils::Strings::to_string(v_str) + '"';

      return Utils::Strings::to_string(v_str);
    }

    case ValueType::Kind::Tuple: {
      auto bak = str_q;

      str_q = true;
      auto ret = "(" + Utils::join(", ", list, [] (auto& p) { return p->to_string(); }) + ")";

      str_q = bak;
      return ret;
    }

    case ValueType::Kind::None:
      return "none";

    case ValueType::Kind::UserDef: {

      break;
    }

  }

  TODO_IMPL
}

//
// clone
Object* Object::clone() const {
  return new Object(*this);
}

} // namespace metro
