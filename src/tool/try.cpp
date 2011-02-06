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
// 1/11  - exploring numeric limits
// 1/11  - integer floor and wrap operation(s)
// 1/11  - how to fetch the path of the own executable -- at least under Linux?


#include <nobug.h>


#include <iostream>
#include <string>

extern "C" {
#include <unistd.h>
}
#include "lib/error.hpp"
#include "lib/symbol.hpp"


using std::string;
using std::cout;
using std::endl;
using lib::Literal;
using lib::STRING_MAX_RELEVANT;

namespace error = lumiera::error;

Literal GET_PATH_TO_EXECUTABLE ("/proc/self/exe");

string
catchMyself ()
{
  string buff(STRING_MAX_RELEVANT+1, '\0' );
  ssize_t chars_read = readlink (GET_PATH_TO_EXECUTABLE, &buff[0], STRING_MAX_RELEVANT);
  
  if (0 > chars_read || chars_read == ssize_t(STRING_MAX_RELEVANT))
    throw error::Fatal ("unable to discover path of running executable");
  
  buff.resize(chars_read);
  return buff;
}


int 
main (int, char**) //(int argc, char* argv[])
  {
    
    NOBUG_INIT;
    
    cout << "\n\nwho am I? :" << catchMyself();
    cout <<  "\n.gulp.\n";
    
    return 0;
  }
