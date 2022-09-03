#include <cstring>
#include "source.h"
#include "Types/Token.h"
#include "MetroDriver/lexer.h"
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
