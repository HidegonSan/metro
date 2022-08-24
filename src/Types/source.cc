#include "source.h"

namespace metro {

Source::Source(std::string_view _path, std::string&& _data)
  : path(_path),
    data(std::move(_data))
{
  LineLoc* line = &lineloc_list.emplace_back();

  line->linenum = 1;

  for( size_t i = 0, num = 1, len = 0; i < data.length(); i++ ) {
    char ch = data[i];

    if( ch == '\n' ) {
      line->end = i;
      line->length = len;

      line = &lineloc_list.emplace_back();
      line->begin = i + 1;
      line->linenum = ++num;

      continue;
    }

    len++;
  }
}

} // namespace metro

