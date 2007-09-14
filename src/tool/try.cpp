/* try.cpp  -  for trying out some language features....
 *             scons will create the binary bin/try
 *
 */ 

// 8/07  - how to control NOBUG??
//         execute with   NOBUG_LOG='ttt:TRACE' bin/try


#include <syslog.h>


#include <nobug.h>

//NOBUG_CPP_DEFINE_FLAG(ttt);
NOBUG_CPP_DEFINE_FLAG_LIMIT(ttt, LOG_WARNING);

int main (int argc, char* argv[])
  {
    
    NOBUG_INIT;
    
    TRACE(ttt,"trace");
    INFO(ttt,"info");
    NOTICE(ttt,"notice");
    WARN(ttt,"warning");
    ERROR(ttt,"error");
    
    TRACE(NOBUG_ON,"allways on?");
    
    return 0;
  }
