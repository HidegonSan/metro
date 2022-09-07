#include <iostream>
#include <iomanip>
#include "Application.h"
#include "Error.h"
#include "Utils.h"

namespace metro {

Error::Error(ErrorKind kind, Token* token, std::string&& msg)
  : kind(kind),
    message(std::forward<std::string>(msg)),
    token(token),
    ast(nullptr),
    pos(0)
{
  debug(assert(token);)
  this->script = Application::get_running_script();
}

Error::Error(ErrorKind kind, AST::Base* ast, std::string&& msg)
  : Error(kind, (Token*)nullptr, std::forward<std::string>(msg))
{
  debug(assert(ast);)
  this->ast = ast;
}

Error::Error(ErrorKind kind, size_t pos, std::string&& msg)
  : Error(kind, (Token*)nullptr, std::forward<std::string>(msg))
{
  this->ast = ast;
  this->pos = pos;
}

Error& Error::add_help(Token* token, std::string&& msg) {
  this->helps.emplace_back(token, std::forward<std::string>(msg));
  return *this;
}

Error& Error::add_help(AST::Base* ast, std::string&& msg) {
  this->helps.emplace_back(ast, std::forward<std::string>(msg));
  return *this;
}

void Error::check() {
  if( emitted_count >= 1 )
    std::exit(1);
}

} // namespace metro

