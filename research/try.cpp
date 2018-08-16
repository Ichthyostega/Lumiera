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
// 7/14  - c++11 transition: std hash function vs. boost hash
// 9/14  - variadic templates and perfect forwarding
// 11/14 - pointer to member functions and name mangling
// 8/15  - Segfault when loading into GDB (on Debian/Jessie 64bit
// 8/15  - generalising the Variant::Visitor
// 1/16  - generic to-string conversion for ostream
// 1/16  - build tuple from runtime-typed variant container
// 3/17  - generic function signature traits, including support for Lambdas
// 9/17  - manipulate variadic templates to treat varargs in several chunks
// 11/17 - metaprogramming to detect the presence of extension points
// 11/17 - detect generic lambda
// 12/17 - investigate SFINAE failure. Reason was indirect use while in template instantiation
// 03/18 - Dependency Injection / Singleton initialisation / double checked locking
// 04/18 - investigate construction of static template members
// 08/18 - Segfault when compiling some regular expressions for EventLog search


/** @file try.cpp
 * Heisenbug hunt: random crashes in BusTerm_test, seemingly emerging from regular expression compilation.
 * Not able to reproduce the crash, unfortunately. //////////////////////////////////////////////////////////TICKET #1158
 */

typedef unsigned int uint;

#include "lib/format-cout.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/util.hpp"

#include <regex>
#include <string>
#include <vector>

using std::string;
using VecS = std::vector<string>;



#define SHOW_TYPE(_TY_) \
    cout << "typeof( " << STRINGIFY(_TY_) << " )= " << lib::meta::typeStr<_TY_>() <<endl;
#define SHOW_EXPR(_XX_) \
    cout << "Probe " << STRINGIFY(_XX_) << " ? = " << _XX_ <<endl;


  


int
main (int, char**)
  {
    VecS rexs{{"after.+_ATTRIBS_.+ins.+1 of 62 ≺293.gen029≻.+mut.+1 of 62 ≺293.gen029≻.+ins.+borgID.+293.+emu.+1 of 62 ≺293.gen029≻"
              ,"after.+_ATTRIBS_.+ins.+1 of 64 ≺251.gen019≻.+mut.+1 of 64 ≺251.gen019≻.+ins.+borgID.+251.+emu.+1 of 64 ≺251.gen019≻"
              ,"after.+_ATTRIBS_.+ins.+1 of 8 ≺203.gen036≻.+mut.+1 of 8 ≺203.gen036≻.+ins.+borgID.+203.+emu.+1 of 8 ≺203.gen036≻"
              ,"after.+?_ATTRIBS_.+?ins.+?53 of 57 ≺358.gen010≻.+?mut.+?53 of 57 ≺358.gen010≻.+?ins.+?borgID.+?358.+?emu.+?53 of 57 ≺358.gen010≻"
              ,"after.+?_ATTRIBS_.+?ins.+?53 of 63 ≺178.gen028≻.+?mut.+?53 of 63 ≺178.gen028≻.+?ins.+?borgID.+?178.+?emu.+?53 of 63 ≺178.gen028≻"
              ,"after.+?_ATTRIBS_.+?ins.+?53 of 59 ≺498.gen038≻.+?mut.+?53 of 59 ≺498.gen038≻.+?ins.+?borgID.+?498.+?emu.+?53 of 59 ≺498.gen038≻"
              ,"after.+?_ATTRIBS_.+?ins.+?53 of 60 ≺223.gen003≻.+?mut.+?53 of 60 ≺223.gen003≻.+?ins.+?borgID.+?223.+?emu.+?53 of 60 ≺223.gen003≻"
              ,"after.+?_ATTRIBS_.+?ins.+?53 of 78 ≺121.gen015≻.+?mut.+?53 of 78 ≺121.gen015≻.+?ins.+?borgID.+?121.+?emu.+?53 of 78 ≺121.gen015≻"
             }};
    for (auto const& str : rexs)
      {
        auto rex = std::regex{str};
        SHOW_EXPR (std::regex_search(str, rex));
      }
    
    
    cout <<  "\n.gulp.\n";
    
    return 0;
  }
