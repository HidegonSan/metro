#pragma once

#include <string>
#include <vector>
#include <locale>
#include <codecvt>

//#define  METRO_DEBUG  1
#define  METRO_DEBUG  (Application::get_instance()->is_debug_enabled())

#define  BIT(n)   (1 << n)

#define  alert  _Alert(__FILE__,__LINE__)
#define  crash  _Crash(__FILE__,__LINE__)

int _Alert(char const* f, int n);
void _Crash(char const* f, int n);

// debugs
// inline static int _Alert(char const* f, int n) {
//   return METRO_DEBUG ? fprintf(stderr,"\t#alert %s:%d\n",f,n) : 0;
// }

// inline static void _Crash(char const* f, int n) {
//   fprintf(stderr,"crashed!! %s:%d\n",f,n);
//   exit(1);
// }

namespace Utils {
  template <class... Args>
  std::string format(char const* fmt, Args&&... args) {
    static char buf[0x1000];
    sprintf(buf, fmt, args...);
    return buf;
  }

  namespace Strings {
    std::u16string to_u16string(std::string const& s);
    std::string to_string(std::u16string const& s);
  };
}