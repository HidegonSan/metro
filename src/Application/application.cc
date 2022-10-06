#include <iostream>
#include <fstream>
#include "Utils.h"
#include "Application.h"

#include "Types.h"

namespace metro {
  static Application* _inst;

  std::list<AppContext::Script*> Application::running_script;

  Application::Application() {
    _inst = this;
  }

  Application::~Application() {

  }

  Application* Application::get_instance() {
    return _inst;
  }

  AppContext const* Application::get_cur_appcontext() {
    return const_cast<AppContext const*>(&_inst->ctx);
  }

  AppContext::Script const* Application::get_running_script() {
    return *running_script.begin();
  }

  void Application::initialize() {
  }

  int Application::main(int argc, char** argv) {
    ctx.argc = argc;
    ctx.argv = argv;

    // try {
      for( auto&& script : ctx.scripts ) {
        if( !ctx.no_print_filenames && ctx.scripts.size() > 1 ) {
          std::cout
            << script.source.path << std::endl;
        }

        execute_script(script);
      }
    // }
    // catch( std::exception e ) {
    //   std::cerr
    //     << std::endl
    //     << "unhandled exception has been occurred: " << e.what() << std::endl;

    //   return -1;
    // }

    return 0;
  }
}
