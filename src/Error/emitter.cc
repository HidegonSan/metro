#include <iostream>
#include <iomanip>
#include "Application.h"
#include "Error.h"
#include "Utils.h"

namespace metro {

size_t _emitted_err_count;

void Error::emit(bool exit) {
  alert;

  std::cout
    << "ERROR: " << this->message << std::endl;

  if( exit ) {
    std::exit(1);
  }
}

//
// エラー出力
void Error::emit_error(Error& err) {
  constexpr auto field = 6;


}

} // namespace metro
