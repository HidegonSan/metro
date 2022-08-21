#include <iostream>
#include <iomanip>
#include "Application.h"
#include "Error.h"
#include "Utils.h"

namespace metro {

//
// pos がある行の番号と範囲を切り取る
// 行端の改行は含みません
//
// タプルの要素 :
//   tuple< 行番号, 開始位置, 終了位置 >
static std::tuple<std::size_t, std::size_t, std::size_t> get_line_from_pos(AppContext::Script const* script, size_t pos) {
  size_t line_num = 1;
  size_t begin = 0;
  size_t end = script->data.length();

  // pos がある行を特定する
  for( size_t i = 0; i < pos; i++ ) {
    auto&& ch = script->data[i];

    // 改行文字
    if( ch ==  '\n' ) {
      // pos より手前
      if( i < pos ) {
        begin = i + 1;
        line_num++; // 行番号を増加
        continue;
      }

      // pos 以降だったら終了位置とする
      end = i - 1;
      break;
    }
  }

  return { line_num, begin, end };
}

//
// エラー出力
void Error::emit_error(Error& err) {
  constexpr auto field = 6;

  auto [line_num, begin, end] = get_line_from_pos(err.script,

  std::cout
    << "error: " << err.message << std::endl
    << "  => " << err.script->path << std::endl
    << std::setw(field) << " | " <<
}

Error::Error(ErrorKind kind, Token* token, std::string&& msg)
  : kind(kind),
    message(std::forward<std::string>(msg)),
    token(token),
    ast(nullptr)
{
  this->script = Application::get_running_script();
}

Error::Error(ErrorKind kind, AST::Base* ast, std::string&& msg)
  : Error(kind, (Token*)nullptr, std::forward<std::string>(msg))
{
  this->ast = ast;
}

Error& Error::add_help(AST::Base* ast, std::string&& msg) {
  this->helps.emplace_back(ast, std::forward<std::string>(msg));
  return *this;
}

void Error::emit(bool exit) {

}


/*
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
}*/


}
