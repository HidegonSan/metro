#include <fstream>
#include "metro.h"

Application* _inst;

Application::Application(bool const dbg)
  : _debug(dbg)
{
  _inst = this;
}

Application* Application::get_instance() {
  return _inst;
}

bool Application::is_debug_enabled() {
  return _debug;
}

int Application::main(int argc, char** argv) {
  
  std::cout << "Hello!" << std::endl;

  std::string source;
  std::ifstream ifs{ "test.txt" };

  // read source
  for( std::string line; std::getline(ifs, line); ) {
    source += line + '\n';
  }

  Lexer lexer{ source };

  auto token = lexer.lex();

  // print token
  for(auto t=token;t->kind!=TOK_END;t=t->next){
    std::cout<<t->str<<std::endl;
  }

  alert;

  Parser parser{ token };

  auto node = parser.parse();

  alert;

  Evaluater eval;
  auto type = eval.eval(node);
  alert;

  std::cout << type.to_string() << std::endl;

  NodeRunner runner;
  auto obj = runner.run(node);

  std::cout << obj->to_string() << std::endl;

  return 0;
}