/* try.cpp  -  for trying out some language features....
 *             scons will create the binary bin/try
 *
 */ 

// 8/07  - how to control NOBUG??
//         execute with   NOBUG_LOG='ttt:TRACE' bin/try


#include <nobug.h>
#include <iostream>
#include <typeinfo>



using std::string;
using std::cout;




int 
main (int argc, char* argv[])
  {
    
    NOBUG_INIT;
    
    cout <<  "\ngulp\n";
    
    
    return 0;
  }
