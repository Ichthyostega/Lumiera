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


//#include <nobug.h>


#include <iostream>
//#include <typeinfo>
#include <string>
//#include <cstdlib>
#include <limits>
#include <cmath>

#include <boost/format.hpp>


using boost::format;
//using std::rand;
using std::string;
using std::cout;
using std::endl;

div_t
floorwrap (int num, int den)
{
  div_t res = div (num,den);
  if (0 > (num^den) && res.rem)
    { // wrap similar to floor()
      --res.quot;
      res.rem = den - (-res.rem);
    }
  return res;
}

void
checkDiv(int lhs, int rhs)
  {
    div_t wrap = floorwrap(lhs,rhs);
    cout << format ("%2d / %2d = %2d %% = % d \tfloor=%6.2f  wrap = (%2d, %2d) \n")
                   % lhs % rhs % (lhs/rhs) % (lhs%rhs) % floor(double(lhs)/rhs) % wrap.quot % wrap.rem; 
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

    checkDiv (0,4);
    checkDiv (0,-4);
    checkDiv (1,4);
    checkDiv (1,-4);
    checkDiv (-1,4);
    checkDiv (-1,-4);
    
    cout << "\n.gulp.\n";
    
    return 0;
  }
