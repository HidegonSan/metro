#include <iostream>
#include <iomanip>
#include "Application.h"
#include "Error.h"
#include "Utils.h"
#include "AST.h"

namespace metro {

size_t _emitted_err_count{ 0 };

void Error::emit(bool exit) {
  alert;

  i64
    view_begin = 0,
    view_end   = script->source.data.length(),
    err_begin  = 0,
    err_end    = view_end;

  std::vector<std::string> view_lines;

  // Token に対するエラー
  if( this->token ) {
    alert;
    err_begin = this->token->pos;
    err_end = this->token->endpos;
  }

  // AST に対するエラー
  else if( this->ast ) {
    alert;

    // std::tie(err_begin, err_end) = ast->get_range_on_source();
    auto [begin_tok, end_tok] = ast->get_range_on_source();

    std::tie(err_begin, err_end) = std::make_tuple(begin_tok->pos, end_tok->endpos);

    alertios(err_begin << " " << err_end);
  }

  // position に対するエラー (ソースコード内の位置)
  else {
    alert;
    err_begin = err_end = this->pos;
  }

  for( auto&& lineloc : this->script->source.lineloc_list ) {
    if( lineloc.end < err_begin )
      continue;

    auto& line = view_lines.emplace_back(this->script->source.data.substr(lineloc.begin, lineloc.length));

    if( lineloc.begin <= err_begin && err_begin <= lineloc.end && lineloc.begin <= err_end && err_end <= lineloc.end ) {
      line.insert(err_end - lineloc.begin, COL_DEFAULT);
      line.insert(err_begin - lineloc.begin, COL_YELLOW);
      break;
    }

    if( lineloc.begin <= err_begin && err_begin <= lineloc.end ) {
      line.insert(err_begin - lineloc.begin, COL_YELLOW);
    }
    else if( lineloc.begin <= err_end && err_end <= lineloc.end ) {
      line.insert(err_end - lineloc.begin, COL_DEFAULT);
      break;
    }
  }

  for(auto&&i:view_lines){
    std::cout<<i<<std::endl;
  }

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
