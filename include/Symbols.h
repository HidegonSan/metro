#pragma once

#include <string>
#include "Types/intdef.h"

namespace metro {

using sym_int_t = u64;

class Symbols {
public:

  static sym_int_t get(std::string_view const& name);

  static std::string_view get_name(sym_int_t sym);

};

} // namespace metro

