#include <iostream>
#include "source.h"
#include "Utils.h"
#include "Application.h"

namespace metro {

std::string Source::LineLoc::to_string() const {
  return Utils::format(
    "{ linenum=%zu, begin=%zu, end=%zu, length=%zu }",
    linenum, begin, end, length
  );
}

Source::Source(std::string_view _path, std::string&& _data)
  : path(_path),
    data(std::move(_data))
{
  this->make_lineloc_list();
}

void Source::make_lineloc_list() {
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

      len = 0;

      continue;
    }

    len++;
  }

  alert;
  alertwhere;
  alertios("print lineloc_list");

  debug(
    for( auto&& line : lineloc_list ) {
      std::cout
        << line << " " << data.substr(line.begin,line.length) << std::endl;
    }
  )
}

} // namespace metro

