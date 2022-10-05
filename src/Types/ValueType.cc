#include "Debug.h"
#include "Types.h"
#include "Utils.h"

namespace metro {

std::vector<std::pair<char const*, ValueType::Kind>> const
    ValueType::name_table{
        {"int", Kind::Int},       {"float", Kind::Float}, {"char", Kind::Char},
        {"string", Kind::String}, {"tuple", Kind::Tuple}, {"none", Kind::None},
    };

bool ValueType::is_no_extents() const { return !is_mutable && !is_reference; }

bool ValueType::equals(ValueType::Kind kind) const {
  return this->kind == kind;
}

bool ValueType::equals(ValueType const& type) const {
  if (kind != type.kind) return false;

  return true;
}

std::string ValueType::to_string() const {
  std::string ret;

  for (auto&& [n, k] : name_table) {
    if (this->kind == k) {
      ret = n;
      break;
    }
  }

  if (!elems.empty()) ret += "<" + Utils::join<ValueType>(", ", elems) + ">";

  for (size_t i = 0; i < arr_depth; i++) ret += "[]";

  if (is_mutable) ret += " mut";

  if (is_reference) ret += "&";

  return ret;
}

}  // namespace metro
