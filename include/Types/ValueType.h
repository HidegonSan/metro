#pragma once

#include <string>
#include <vector>

#define BIT(N)  (1 << N)

namespace Metro {
  namespace AST {
    struct Struct;
  }

  struct ValueType {
    enum class Kind {
      Int,
      Float,
      Bool,
      Char,
      String,
      Tuple,
      UserDef,
      Args,
      None
    };

    Kind        kind;
    size_t      arr_depth;
    bool        have_elements;
    std::vector<ValueType>   elems;
    AST::Struct*  structptr;

    bool  is_mutable;
    bool  is_reference;

    ValueType(Kind kind = Kind::None)
      : kind(kind),
        arr_depth(0),
        have_elements(false),
        structptr(nullptr),
        is_mutable(false),
        is_reference(false)
    {
    }

    bool is_no_extents() const;

    bool equals(Kind kind) const;
    bool equals(ValueType const& type) const;

    std::string to_string() const;

    static std::vector<std::pair<char const*, Kind>> const name_table;
  };
}