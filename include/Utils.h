#pragma once

#include <concepts>
#include <string>
#include <vector>
#include <locale>
#include <codecvt>
#include <sstream>
#include "Debug.h"
#include "macro.h"

template <class T>
concept contains_to_string_method = requires (T const& x) {
  { x.to_string() } -> std::convertible_to<std::string>;
};

namespace Utils {
  template <class... Args>
  std::string format(char const* fmt, Args&&... args) {
    static char buf[0x1000];
    sprintf(buf, fmt, args...);
    return buf;
  }

  template <class... Args>
  std::string linkstr(Args&&... args) {
    std::stringstream ss;

    (ss << ... << args);

    return ss.str();
  }

  template <contains_to_string_method T>
  std::string join(std::string const& s, std::vector<T> const& vec) {
    std::string ret;

    for( auto last = &*vec.rbegin(); auto&& x : vec ) {
      ret += x.to_string();
      if( last != &x ) ret += s;
    }

    return ret;
  }

  template <class T>
  std::string join(std::string const& s, std::vector<T> const& vec, auto conv) {
    std::string ret;

    for( auto last = &*vec.rbegin(); auto&& x : vec ) {
      ret += conv(x);
      if( last != &x ) ret += s;
    }

    return ret;
  }

  template <class T>
  std::string join(std::string const& s, std::vector<T> const& vec) {
    std::string ret;

    for( auto it = vec.begin(); it != vec.end(); it++ ) {
      if constexpr( std::is_convertible_v<T, std::string>) {
        ret += *it;
      }
      else if constexpr( std::is_constructible_v<std::string, T> ) {
        ret += std::string{ *it };
      }
      else {
        ret += std::to_string(*it);
      }

      if( it < vec.end() - 1 ) ret += s;
    }

    return ret;
  }

  namespace Strings {
    std::u16string to_u16string(std::string const& s);
    std::string to_string(std::u16string const& s);
  }
}