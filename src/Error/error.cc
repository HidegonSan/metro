#include <iostream>
#include "Application.h"
#include "Error.h"
#include "Utils.h"

namespace Metro {
  void ErrorContext::show() {
    alert;

    auto spacec = err_begin - view_begin;

    std::cerr
      << Utils::format("%6zu | ", linenum) << script->data.substr(view_begin, view_end - view_begin) << std::endl
      << "       | " << std::string(spacec, ' ') << std::string(err_underline_len, '^') << ' ' << message << std::endl;
  }

  void Error::show_all() {
    for( auto&& ctx : contexts ) {
      ctx.show();
    }
  }

  void Error::check() {
    if( !contexts.empty() ) {
      exit_app();
    }
  }

  void Error::exit_app() {
    show_all();
    exit(1);
  }
}