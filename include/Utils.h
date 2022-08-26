#pragma once

#include <codecvt>
#include <concepts>
#include <functional>
#include <locale>
#include <sstream>
#include <string>
#include <vector>

#include "Debug.h"
#include "macro.h"

using std::to_string;

template <class callable_t>
using return_type_of_t =
  typename decltype(std::function{ std::declval<callable_t>(0) }())::result_type;

template <class T>
concept convertible_to_string_with_method = requires (T const& x) {
  { x.to_string() } -> std::convertible_to<std::string>;
};

template <convertible_to_string_with_method T>
inline auto to_string(T const& x) -> std::string {
  return x.to_string();
}

template <class T>
concept convertible_to_string =
  convertible_to_string_with_method<T> || std::is_convertible_v<T, std::string>;

template <convertible_to_string_with_method T>
std::ostream& operator << (std::ostream& ost, T const& x) {
  return ost << x.to_string();
}

namespace Utils {

/* append all elements in param-b into param-a */
template <class T, class V = std::vector<T>>
auto append_vec(V& a, V const& b) -> V& {
  for( auto&& e : b )
    a.emplace_back(e);

  return a;
}

/*  */
template <class... Args>
auto format(char const* fmt, Args&&... args) -> std::string {
  static char buf[0x1000];
  sprintf(buf, fmt, args...);
  return buf;
}

template <class... Args>
auto linkstr(Args&&... args) -> std::string {
  std::stringstream ss;

  (ss << ... << args);

  return ss.str();
}

template <convertible_to_string T>
std::string join(std::string const& s, std::vector<T> const& vec) {
  std::string ret;

  for( auto last = &*vec.rbegin(); auto&& x : vec ) {
    ret += to_string(x);
    if( last != &x ) ret += s;
  }

  return ret;
}

template <class T, class F = std::function<std::string(T)>>
std::string join(std::string const& s, std::vector<T> const& vec, F conv) {
  std::string ret;

  for( auto last = &*vec.rbegin(); auto&& x : vec ) {
    ret += conv(x);
    if( last != &x ) ret += s;
  }

  return ret;
}

namespace Strings {

std::u16string to_u16string(std::string const& s);
std::string to_string(std::u16string const& s);

} // namespace Utils::Strings

} // namespace Utils

