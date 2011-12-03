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
// 10/11 - simple demo using a pointer and a struct
// 11/11 - using the boost random number generator(s)


#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/random/linear_congruential.hpp>

using boost::lexical_cast;

using std::cout;
using std::endl;



int 
main (int cnt, char* argv[])
  {
    
    int32_t seed = (2 == cnt)? lexical_cast<int32_t> (argv[1]) : 42;
    
    boost::rand48 ranGen(seed);
    
    cout << "seed = "<< seed << endl;
    for (uint i=0; i< 100; ++i)
      cout << ranGen() % CHAR_MAX <<"__";
    
    cout <<  "\n.gulp.\n";
    
    return 0;
  }
