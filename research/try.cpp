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


/** @file try.cpp
 ** Investigation: how to supply a hash function for custom types.
 ** This example defines two custom types, each of which provides a way
 ** to calculate hash values. But in one case, we use the new \c std::hash
 ** as a framework, in the other case we use the extension mechanism for
 ** \c boost::hash. The latter has the benefit of being simpler and less verbose
 ** to write, since a simple ADL function is sufficient as extension point
 ** 
 */


//#include <type_traits>
#include <iostream>
#include <string>
#include <vector>

#include <boost/functional/hash.hpp>
#include <functional>

using std::vector;
using std::string;
using std::cout;
using std::endl;

class S
  {
    string s_;
    
    friend std::hash<S>;
    
  public:
    S(string ss ="")
      : s_(ss)
      { }
  };

namespace std {
  template<>
  struct hash<S>
    {
      size_t
      operator() (S const& val)  const noexcept
        {
          hash<string> string_hasher;
          return string_hasher(val.s_);
        }
    };
}

class V
  {
    vector<string> v_;
    
  public:
    V(string ss ="")
      {
        v_.push_back(ss);
      }
    
    friend size_t
    hash_value (V const& v)
    {
      return boost::hash_value(v.v_);
    }
  };


int 
main (int, char**)
  {
    string p("Путин"), pp(p);
    S s(p), ss(pp);
    V v(p), vv(pp);
    
    std::hash<string>   std_stringHasher;
    boost::hash<string> boo_stringHasher;
    
    std::hash<S>   std_customHasher;
    boost::hash<V> boo_customHasher;
    
    cout << "raw hash(std) = "        << std_stringHasher(p) <<"|"<< std_stringHasher(pp)
         << " (boost) = "             << boo_stringHasher(p) <<"|"<< boo_stringHasher(pp)
         << "\n custom hash (std)   " << std_customHasher(s) <<"|"<< std_customHasher(ss)
         << "\n custom hash (boost) " << boo_customHasher(v) <<"|"<< boo_customHasher(vv)
         ;
    cout <<  "\n.gulp.\n";
    
    return 0;
  }
