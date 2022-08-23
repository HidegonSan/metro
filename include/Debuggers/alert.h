#pragma once

#include <cstring>

#define  __FILE_EX__ __file_ex_fn__(__FILE__, "src")

// alert
#define  alert  \
  fprintf(stderr,COL_MAGENTA "\t#alert %s:%d\n" COL_DEFAULT,__FILE_EX__,__LINE__)

// alertmsg
#define  alertmsg(e...) \
  fprintf(stderr,COL_YELLOW "\t#message: " \
    COL_ALERTMSG #e COL_YELLOW " :from %s:%d\n" COL_DEFAULT,__FILE_EX__,__LINE__)

// alertfmt
#define  alertfmt(fmt, e...) \
  fprintf(stderr,COL_YELLOW "\t#message: " fmt \
    COL_ALERTMSG #e COL_YELLOW " :from %s:%d\n" COL_DEFAULT,e,__FILE_EX__,__LINE__)

// alertios
#define  alertios(e...) \
  (std::cerr<<COL_YELLOW"\t#message: " \
    COL_ALERTMSG<<e<<COL_YELLOW" :from " << __FILE_EX__ << ":"<<__LINE__<<"\n" COL_DEFAULT)

// alertwarn
#define  alertwarn(e...) \
  alertmsg(COL_RED "#warning: " #e)

// alertctor
#define  alertctor(_Name_) \
  fprintf(stderr,COL_GREEN "\t#Constructing " \
    COL_CYAN #_Name_ " (%p)" \
    COL_GREEN":%s:%d\n" COL_DEFAULT,this,__FILE_EX__,__LINE__)

// alertctor
#define  alertdtor(_Name_) \
  fprintf(stderr,COL_RED "\t#Destructing " \
    COL_CYAN #_Name_ " (%p)" \
    COL_RED":%s:%d\n" COL_DEFAULT,this,__FILE_EX__,__LINE__)

// alertwhere
#define  alertwhere \
  fprintf(stderr,"\t" COL_MAGENTA "# here is in function " \
  COL_YELLOW "'%s'" COL_MAGENTA " in " COL_GREEN "%s\n" COL_DEFAULT,__func__,__FILE_EX__)

inline char const* __file_ex_fn__(char const* a, char const* b) {
  size_t const len = strlen(b);

  for( auto p = a;; )
    if( memcmp(++p, b, len) == 0 )
      return p;

  return a;
}

