#pragma once

#include <cassert>
#include <iostream>
#include "colors.h"

#define METRO_DEBUG  1

// color in message area in alert macro
#define  COL_ALERTMSG  COL_WHITE

#if METRO_DEBUG
  #include "Debuggers/alert.h"
  #define  debug(e...) { e }
  #define  crash \
    {alert;fprintf(stderr,"\n#crashed at " __FILE__ ":%d\n",__LINE__);exit(1);}
#else
  #define  alert              0
  #define  alertmsg(...)      0
  #define  alertfmt(...)      0
  #define  alertios(...)      0
  #define  alertwarn(...)     0
  #define  alertctor(__x)     0
  #define  alertdtor(__x)     0
  #define  alertwhere         0
  #define  debug(...)  { }
  #define  crash \
    {fprintf(stderr,COL_RED "\n#------------------------------------#" \
    "crashed at %s:%d\nPlease tell to developer.\n" COL_DEFAULT,__FILE__,__LINE__);exit(1);}
#endif

#define  TODO_IMPL  \
  {fprintf(stderr,COL_RED "\n\n# Not implemented error at " \
  COL_YELLOW "%s:%d\n" COL_DEFAULT,__FILE__,__LINE__);exit(1);}

/*
namespace metro {
  namespace Debug {
  }
}
*/

