#include <iostream>
#include <iomanip>
#include "Application.h"
#include "Error.h"
#include "Utils.h"
#include "AST.h"

// white + bold + underline
#define COL_ERROR "\033[37;1;4m"

namespace metro {

size_t Error::emitted_count{ 0 };

static char const* err_kind_string_table[] = {
  "invalid token",
  "invalid syntax",
  "uninitialized value",
  "unexpected token",
  "not allowed expression",
  "not mutable",
  "undefined variable name",
  "undefined function name",
  "undefined type name",
  "'if' without 'else'",
  "multiple definition",
  "cannot infer type",
  "type mismatch",
  "too few arguments",
  "too many arguments",
  "invalid arguments",
  "empty struct",
  "stack overflow",
};

void Error::emit(bool exit) {
  i64
    view_begin = 0,
    view_end   = script->source.data.length(),
    err_begin  = 0,
    err_end    = view_end;

  std::vector<std::pair<size_t, std::string>> view_lines;

  Source::LineLoc const* last_view_line = nullptr;

  // Token に対するエラー
  if( this->token ) {
    err_begin = this->token->pos;
    err_end = this->token->endpos;
  }

  // AST に対するエラー
  else if( this->ast ) {
    // std::tie(err_begin, err_end) = ast->get_range_on_source();
    auto [begin_tok, end_tok] = ast->get_range_on_source();

    std::tie(err_begin, err_end) = std::make_tuple(begin_tok->pos, end_tok->endpos);
  }

  // position に対するエラー (ソースコード内の位置)
  else {
    err_begin = err_end = this->pos;
  }

  // ソースコード切り取り ( エラー表示用 )
  for( auto&& lineloc : this->script->source.lineloc_list ) {
    if( lineloc.end < err_begin )
      continue;

    last_view_line = &lineloc;

    auto& line = view_lines.emplace_back(lineloc.linenum, this->script->source.data.substr(lineloc.begin, lineloc.length));

    auto _err_attr_begin = [&line, &lineloc, &err_begin, &err_end] () {
      if( Utils::in_range(lineloc.begin, lineloc.end, err_begin) )
        line.second.insert(err_begin - lineloc.begin, COL_ERROR);
      else
        for( size_t i = 0; auto&& c : line.second ) {
          if( c > ' ' ) {
            line.second.insert(i, COL_ERROR);
            break;
          }

          i++;
        }
    };

    auto _err_attr_end = [&line, &lineloc, &err_begin, &err_end] () {
      if( Utils::in_range(lineloc.begin, lineloc.end, err_end) )
        line.second.insert(err_end - lineloc.begin, COL_DEFAULT);
      else
        line.second.append(COL_DEFAULT);
    };

    _err_attr_end();
    _err_attr_begin();

    if( err_end <= lineloc.end )
      break;
  }

  auto errkindtext = err_kind_string_table[static_cast<u32>(kind)];

  std::cerr
    << "\033[31;1m" << "Error: " << errkindtext << std::endl
    << COL_CYAN << "  --> in " << script->source.path << COL_DEFAULT << std::endl
    << "       | " << std::endl;

  for( auto&& line : view_lines )
    std::cerr << COL_DEFAULT << std::setw(6) << line.first << " | " << line.second << std::endl;

  std::cerr
    << "       | " << std::string(err_end - last_view_line->begin, ' ') << this->message << std::endl;

  if( exit ) {
    std::exit(1);
  }

  emitted_count += 1;
}

} // namespace metro
