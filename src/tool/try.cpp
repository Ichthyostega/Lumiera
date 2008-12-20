/* try.cpp  -  for trying out some language features....
 *             scons will create the binary bin/try
 *
 */ 

// 8/07  - how to control NOBUG??
//         execute with   NOBUG_LOG='ttt:TRACE' bin/try
// 1/08  - working out a static initialisation problem for Visitor (Tag creation)
// 1/08  - check 64bit longs
// 4/08  - comparison operators on shared_ptr<Asset>
// 4/08  - conversions on the value_type used for boost::any
// 5/08  - how to guard a downcasting access, so it is compiled in only if the involved types are convertible
// 7/08  - combining partial specialisation and subclasses 
// 10/8  - abusing the STL containers to hold noncopyable values


#include "include/nobugcfg.h"

#include <iostream>

using std::string;
using std::cout;


int 
main (int argc, char* argv[])
  {
    
    NOBUG_INIT;
    
    cout <<  "\n.gulp.\n";
   
   int * my = 0;
   int oh = *my;  // Congratulations...
    
    return 0;
  }
