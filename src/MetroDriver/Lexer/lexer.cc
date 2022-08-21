#include <cstring>
#include "Source.h"
#include "Types/Token.h"
#include "MetroDriver/Lexer.h"
#include "Utils.h"

namespace metro {
  Lexer::Lexer(Source const& src)
    : source(source),
      position(0),
      length(source.length()),
      linenum(0)
  {
  }
}
