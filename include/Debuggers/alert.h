#pragma once

// alert
#define  alert  \
  fprintf(stderr,COL_MAGENTA "\t#alert " __FILE__ ":%d\n" COL_DEFAULT,__LINE__)

// alertmsg
#define  alertmsg(e...) \
  fprintf(stderr,COL_YELLOW "\t#message: " \
    COL_ALERTMSG #e COL_YELLOW " :from " __FILE__ ":%d\n" COL_DEFAULT,__LINE__)

// alertfmt
#define  alertfmt(fmt, e...) \
  fprintf(stderr,COL_YELLOW "\t#message: " fmt \
    COL_ALERTMSG #e COL_YELLOW " :from " __FILE__ ":%d\n" COL_DEFAULT,e,__LINE__)

// alertios
#define  alertios(e...) \
  (std::cerr<<COL_YELLOW"\t#message: " \
    COL_ALERTMSG<<e<<COL_YELLOW" :from " __FILE__ ":"<<__LINE__<<"\n" COL_DEFAULT)

// alertwarn
#define  alertwarn(e...) \
  alertmsg(COL_RED "warning: " #e)

// alertctor
#define  alertctor(_Name_) \
  fprintf(stderr,COL_GREEN"\t#Constructing " \
    COL_CYAN #_Name_ " (%p)" \
    COL_GREEN":" __FILE__":%d\n" COL_DEFAULT,this,__LINE__)

// alertwhere
#define  alertwhere \
  fprintf(stderr,"\t" COL_MAGENTA "# here is in function " \
  COL_YELLOW "'%s'" COL_MAGENTA " in " COL_GREEN __FILE__ "\n" COL_DEFAULT,__func__)


