#pragma once

#include <string>
#include <utility>

namespace metro {

struct Source {
  std::string_view path;

  std::string data;

  std::vector<std::pair<size_t, size_t>> linepos_list;

  explicit Source(std::string_view path, std::string&& data)
    : path(path),
      data(std::move(data))
  {
  }
};

} // namespace metro

