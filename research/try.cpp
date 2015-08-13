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


/** @file try.cpp
 ** Investigation: Segfault when loading into GDB (on Debian/Jessie 64bit)
 **
 */

#include "lib/test/test-helper.hpp"
//#include "lib/format-util.hpp"
#include "lib/meta/trait.hpp"
#include "lib/itertools.hpp"
#include "lib/symbol.hpp"

#include <iostream>
#include <string>
#include <vector>

using std::string;
using std::vector;
using std::cout;
using std::endl;


  namespace { // helper to build range iterator on demand
    template<class CON, typename TOGGLE = void>
    struct _RangeIter
      {
        using StlIter = typename CON::const_iterator;
        
        lib::RangeIter<StlIter> iter;
        
        _RangeIter(CON const& collection)
          : iter(begin(collection), end(collection))
          { }
      };
    
  }


template<class CON>
inline string
join (CON&& coll, string const& delim =", ")
{
  using Coll = typename lib::meta::Strip<CON>::Type;
  using Val =  typename Coll::value_type;
  
  std::function<string(Val const&)> toString = [] (Val const& val) { return string(val); };
  
  _RangeIter<Coll> range(std::forward<Coll>(coll));
  auto strings = lib::transformIterator(range.iter, toString);
  
  if (!strings) return "";
  
  std::ostringstream buffer;
  for (string const& elm : strings)
      buffer << elm << delim;
  
  // chop off last delimiter
  size_t len = buffer.str().length();
  ASSERT (len > delim.length());
  return buffer.str().substr(0, len - delim.length());
}



int
main (int, char**)
  {
    vector<string> crew;
    crew.push_back("Picard");
    crew.push_back("Riker");
    crew.push_back("Data");
    crew.push_back("Troi");
    crew.push_back("Worf");
    crew.push_back("Crusher");
    crew.push_back("La Forge");
    
    cout << "enterprise = " << join(crew)<<endl;
    
    cout <<  "\n.gulp.\n";
    
    return 0;
  }
