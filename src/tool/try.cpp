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
// 6/09  - investigating how to build a mixin template providing an operator bool()
// 12/9  - tracking down a strange "warning: type qualifiers ignored on function return type"
// 1/10  - can we determine at compile time the presence of a certain function (for duck-typing)?
// 4/10  - pretty printing STL containers with python enabled GDB?


//#include <nobug.h>
//#define LUMIERA_LOGGING_CXX
//#include "include/logging.h"
//#include "include/nobugcfg.h"


#include <iostream>
//#include <typeinfo>
#include <string>
#include <vector>
#include <cstdlib>
#include <cstdio>


//using std::rand;
using std::string;
using std::cout;
using std::endl;


int 
main (int, char**)
  {
    
//  NOBUG_INIT;
    
    std::string str = "hullo wöld";
    std::vector<int> vec (1000);
    
    for (uint i = 0; i < vec.size(); ++i)
        vec[i] = i;
    
    cout << str <<  "\n.gulp.\n";
    
    // Note: when selecting the string or the vector in the Eclipse variables view
    //       (or when issuing "print str" at the GDB prompt), the GDB python pretty-printer
    //       should be activated. Requires an python enabled GDB (>6.8.50). Debian/Lenny isn't enough,
    //       but compiling the GDB package from Debian/Squeeze (GDB-7.1) is straightforward.
    //       Moreover, you need to check out and install the python pretty-printers and 
    //       you need to activate them through your ~/.gdbinit
    //       see http://sourceware.org/gdb/wiki/STLSupport
    
    return 0;
  }
