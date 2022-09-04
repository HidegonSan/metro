#include <iostream>
#include <fstream>
#include <cassert>
#include "Utils.h"
#include "Types.h"

#include "MetroDriver/lexer.h"
#include "MetroDriver/Parser.h"
// #include "MetroDriver/sema.h"
#include "MetroDriver/Sema.h"
#include "MetroDriver/evaluator.h"

#include "Application.h"
#include "Error.h"
#include "Debug.h"

namespace metro {

AppContext::Script Application::open_script_file(char const* path) {
  std::ifstream ifs{ path };
  AppContext::Script script;

  script.source.path = path;

  if( ifs.fail() ) {
    std::cout << "cannot open file: " << path << std::endl;
    exit(1);
  }

  for( std::string line; std::getline(ifs, line); ) {
    script.source.data += line + '\n';
  }

  script.source.make_lineloc_list();

  return script;
}

Object* Application::execute_script(AppContext::Script& script) {
  running_script.push_front(&script);

  Lexer lexer{ script.source };

  auto token = lexer.lex();

  Error::check();

  Parser parser{ token };

  auto ast = parser.parse();

  debug(
    std::cout << ast->to_string() << std::endl;
  )

  script.ast = ast;

  Error::check();

  assert(ast->kind == AST::Kind::Scope);

/*
  Sema sema{ (AST::Scope*)ast };

  sema.analyze();
  */

  semantics::Sema sema{ (AST::Scope*)ast };

  sema.analyze();

  Error::check();

  return nullptr;
  // ---------------

  Evaluator evaluator;

  auto obj = evaluator.eval(ast);

  alertios("evaluated result: " << obj->to_string());

  running_script.pop_front();

  return obj;
}

} // namespace metro
