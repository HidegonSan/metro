#pragma once

#include <string>
#include <vector>
#include <utility>

namespace metro {

struct Source {

  struct LineLoc {
    size_t linenum = 0;
    size_t begin = 0;
    size_t end = 0;
    size_t length = 0;
  };

  std::string_view path;

  std::string data;

  std::vector<LineLoc> lineloc_list;

  explicit Source(std::string_view _path, std::string&& _data);
};

} // namespace metro

