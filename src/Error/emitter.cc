#include <iomanip>
#include <iostream>
#include <map>

#include "AST.h"
#include "Application.h"
#include "Error.h"
#include "Utils.h"

// white + bold + underline
#define COL_ERROR "\033[37;1;4m"

namespace metro {

size_t Error::emitted_count{0};

// static char const* err_kind_string_table[] = {
std::map<ErrorKind, char const*> err_kind_str_map{
    {ErrorKind::LanguageVersion, "version error"},
    {ErrorKind::InvalidToken, "invalid token"},
    {ErrorKind::InvalidSyntax, "invalid syntax"},
    {ErrorKind::UninitializedValue, "uninitialized value"},
    {ErrorKind::UnexpectedToken, "unexpected token"},
    {ErrorKind::NotAllowed, "not allowed expression"},
    {ErrorKind::NotMutable, "not mutable"},
    {ErrorKind::UndefinedVariable, "undefined variable name"},
    {ErrorKind::UndefinedFunction, "undefined function name"},
    {ErrorKind::UndefinedTypeName, "undefined type name"},
    {ErrorKind::IfWithoutElse, "'if' without 'else'"},
    {ErrorKind::MultipleDefinition, "multiple definition"},
    {ErrorKind::CannotInfer, "cannot infer type"},
    {ErrorKind::TypeMismatch, "type mismatch"},
    {ErrorKind::TooFewArguments, "too few arguments"},
    {ErrorKind::TooManyArguments, "too many arguments"},
    {ErrorKind::InvalidArguments, "invalid arguments"},
    {ErrorKind::EmptyStruct, "empty struct"},
    {ErrorKind::StackOverflow, "stack overflow"},
    {ErrorKind::ApplicationBug, "application bug"},
    {ErrorKind::UnusedVariable, "unused variable"}};

auto trim_view_lines(AppContext::Script const* script, size_t err_begin,
                     size_t err_end) {
  std::vector<std::tuple<Source::LineLoc const*, std::string>> ret;

  for (auto&& lineloc : script->source.lineloc_list) {
    if (lineloc.end < err_begin)
      continue;
    else if (err_end < lineloc.begin)
      break;

    auto& elem = ret.emplace_back(
        &lineloc, script->source.data.substr(lineloc.begin, lineloc.length));

    auto& num = std::get<0>(elem);
    auto& line = std::get<1>(elem);

    if (err_begin < lineloc.begin && lineloc.end < err_end) {
      line = COL_ERROR + line + COL_DEFAULT;
      continue;
    }

    if (lineloc.begin <= err_end && err_end <= lineloc.end)
      line.insert(err_end - lineloc.begin, COL_DEFAULT);

    if (lineloc.begin <= err_begin && err_begin <= lineloc.end)
      line.insert(err_begin - lineloc.begin, COL_ERROR);
    else
      line.insert(0, COL_ERROR);
  }

  return ret;
}

Error& Error::emit(bool exit) {
  i64 view_begin = 0, view_end = script->source.data.length(), err_begin = 0,
      err_end = view_end;

  // Token に対するエラー
  if (this->token) {
    err_begin = this->token->pos;
    err_end = this->token->endpos;
  }

  // AST に対するエラー
  else if (this->ast) {
    // std::tie(err_begin, err_end) = ast->get_range_on_source();
    auto [begin_tok, end_tok] = ast->get_range_on_source();

    std::tie(err_begin, err_end) =
        std::make_tuple(begin_tok->pos, end_tok->endpos);
  }

  // position に対するエラー (ソースコード内の位置)
  else {
    err_begin = err_end = this->pos;
  }

  debug(assert(err_kind_str_map.contains(kind));)

      auto&& errkindtext = err_kind_str_map[kind];
  auto&& view_lines = trim_view_lines(this->script, err_begin, err_end);

  // warning
  if (this->is_warn) {
    std::cerr << "\033[35;1m"
              << "warning: " << errkindtext << std::endl;
  }
  // error
  else {
    auto&& errkindtext = err_kind_str_map[kind];

    std::cerr << "\033[31;1m"
              << "Error: " << errkindtext << std::endl;
  }

  std::cerr << COL_CYAN << "  --> in " << script->source.path << COL_DEFAULT
            << std::endl
            << "       | " << std::endl;

  // ソースコード表示
  for (auto&& line : view_lines)
    std::cerr << COL_DEFAULT << std::setw(6) << std::get<0>(line)->linenum
              << " | " << std::get<1>(line) << std::endl;

  // メッセージ表示
  std::cerr << "       | "
            << std::string(err_end - std::get<0>(*view_lines.rbegin())->begin,
                           ' ')
            << this->message << std::endl
            << std::endl;

  //
  // ヘルプを表示
  for (auto&& help : this->helps) {
    auto&& range = help.ast ? help.ast->get_range_on_source()
                            : std::make_pair(help.token, help.token);

    std::tie(err_begin, err_end) =
        std::make_tuple(range.first->pos, range.second->endpos);

    view_lines = std::move(trim_view_lines(this->script, err_begin, err_end));

    for (auto&& line : view_lines)
      std::cerr << COL_DEFAULT << std::setw(6) << std::get<0>(line)->linenum
                << " | " << std::get<1>(line) << std::endl;

    std::cerr << "       | "
              << std::string(err_end - std::get<0>(*view_lines.rbegin())->begin,
                             ' ')
              << "help: " << help.msg << std::endl
              << std::endl;
  }

  if (!this->is_warn) emitted_count += 1;

  if (exit || emitted_count >= 10) {
    std::exit(1);
  }

  if (kind == ErrorKind::ApplicationBug) {
    std::cerr << COL_RED << "this error is bug!! please tell to developer...\n"
              << "  GitHub: https://github.com/bomkei/metro\n"
              << "  Discord: https://discord.gg/PPujBK4fdu\n"
              << COL_DEFAULT;

    std::exit(1);
  }

  return *this;
}

}  // namespace metro
