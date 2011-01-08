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


//#include <nobug.h>


#include <iostream>
//#include <typeinfo>
#include <string>
//#include <cstdlib>
#include <limits>

#include <boost/format.hpp>


using boost::format;
//using std::rand;
using std::string;
using std::cout;
using std::endl;

void
checkDiv(int lhs, int rhs)
  {
    cout << format ("%f / %f = %f \n") % lhs % rhs % (lhs / rhs); 
  }

int 
main (int, char**)
  {
    
//  NOBUG_INIT;
    
    checkDiv (8,4);
    checkDiv (9,4);
    checkDiv (-8,4);
    checkDiv (-9,4);
    checkDiv (8,-4);
    checkDiv (9,-4);
    checkDiv (-8,-4);
    checkDiv (-9,-4);
    
    
    int64_t muks = std::numeric_limits<int64_t>::max();
    muks /= 30;
    double murks(muks);
    
    cout << format("%f // %f || %g \n") % muks % murks % std::numeric_limits<double>::epsilon();
    
    int64_t glucks = murks;
    cout << glucks <<endl;
    
    cout << "\n.gulp.\n";
    
    return 0;
  }
