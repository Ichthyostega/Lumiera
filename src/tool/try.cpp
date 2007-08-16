/* try.cpp  -  for trying out some language features....
 *             scons will create the binary bin/try
 *
 */ 

// 8/07  - how to control NOBUG??


#include <syslog.h>

#define NOBUG_LOG_LIMIT LOG_ERR

#include <nobug.h>

NOBUG_DECLARE_FLAG(ttt);

NOBUG_DEFINE_FLAG(ttt);

int main (int argc, char* argv[])
  {
    
    NOBUG_INIT;
    
    NOBUG_INIT_FLAG_LIMIT(ttt, LOG_WARNING);
    
    TRACE(ttt,"trace");
    INFO(ttt,"info");
    NOTICE(ttt,"notice");
    WARN(ttt,"warning");
    ERROR(ttt,"error");
    
    TRACE(NOBUG_ON,"allways on?");
    
    return 0;
  }
