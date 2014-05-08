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
// 12/11 - how to detect if string conversion is possible?
// 1/12  - is partial application of member functions possible?
// 5/14  - c++11 transition: detect empty function object


/** @file try.cpp
 ** Investigation: empty and unbound function objects.
 ** Since \c std::function is bool convertible, it should be possible to detect an empty or
 ** unbound functor object and record this state in a VTable. Actually this approach used to
 ** work with tr1::function objects. But it ceased to work after switching to c++11
 ** 
 ** The reason is the more concise meaning of \em convertibility with C++11 -- now, an
 ** automatic conversion is required; thus what we need is rather the ability to \em construct
 ** our target type from the given source explicitly, which is a weaker requirement.
 ** 
 */


#include <type_traits>
#include <functional>
#include <iostream>

//using std::placeholders::_1;
//using std::placeholders::_2;
using std::function;
using std::bind;

using std::string;
using std::cout;
using std::endl;


uint
funny (char c)
{
  return c;
}

using FUC = function<uint(char)>;

int 
main (int, char**)
  {
    FUC fun(funny);
    FUC empty;
    
    cout << "ASCII 'A' = " << fun('A');
    cout << " defined: " << bool(fun)
         << " undefd; " << bool(empty)
         << " bool-convertible: " << std::is_convertible<FUC, bool>::value
         << " can build bool: " << std::is_constructible<bool,FUC>::value
         << " bool from string: " << std::is_constructible<bool,string>::value;
    
    cout <<  "\n.gulp.\n";
    
    return 0;
  }
