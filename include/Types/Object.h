#pragma once

#include <string>
#include "TypeInfo.h"

struct Object {
  TypeInfo    type;
  size_t      ref_count;
  bool        is_weak;

  union {
    int64_t     v_int;
    double      v_float;
    bool        v_bool;
    char16_t    v_char;
  };
  
  std::u16string  v_str;

  static Object* none;

  Object(TypeInfo type = TYPE_NONE)
    : type(type),
      ref_count(0),
      is_weak(false)
  {
  }

  std::string to_string() const;
};