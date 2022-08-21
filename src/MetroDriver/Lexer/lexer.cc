#include <cstring>
#include "Source.h"
#include "Types/Token.h"
#include "MetroDriver/Lexer.h"
#include "Utils.h"

namespace metro {
  Lexer::Lexer(Source const& src)
    : position(0),
      linenum(0),
      source(src.data),
      length(src.data.length()),
      src(src)
  {
  }
}
