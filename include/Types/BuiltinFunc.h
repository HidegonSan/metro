#pragma once

/* -------------------- //
  builtin-function
// --------------------- */

#include <vector>
#include "ValueType.h"

namespace metro {
  struct Object;
  struct BuiltinFunc {
    using FuncPointer = Object*(* const)(std::vector<Object*> const& args);

    char const*             name;
    ValueType               ret_type;
    std::vector<ValueType>  arg_types;
    FuncPointer             func;

    static std::vector<BuiltinFunc> const builtin_functions;
  };
}
