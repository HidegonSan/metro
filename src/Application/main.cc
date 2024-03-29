#include <fstream>
#include "Application.h"

int main(int argc, char** argv) {
  auto app = metro::Application{ };

  app.initialize();

  if( !app.parse_args(argc, argv) ) {
    return 0;
  }

  return app.main(argc, argv);
}
