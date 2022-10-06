#pragma once

#include <string>
#include <vector>

#include "Debug.h"
#include "ValueType.h"

namespace metro {

struct Object {
  ValueType type;
  size_t ref_count;
  bool is_weak;

  union {  // dont use
    int64_t v_int;
    double v_float;
    bool v_bool;
    char16_t v_char;
  };

  std::u16string v_str;
  std::vector<Object*> list;

  static Object* none;

  Object(ValueType type = ValueType::Kind::None);
  virtual ~Object();

  virtual std::string to_string() const;

  Object* clone() const;
};

struct MeLong : Object {
  int64_t v_long;

  // ctor
  MeLong(int64_t v = 0) : Object(ValueType::Kind::Int), v_long(v) {}

  std::string to_string() const;
};

struct MeString : Object {
  std::wstring value;

  MeString(std::wstring const& v = L"")
      : Object(ValueType::Kind::String), value(v) {}

  std::string to_string() const;
};

}  // namespace metro
