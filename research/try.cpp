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


#include "lib/util.hpp"

#include <iostream>
#include <string>

using std::string;
using std::cout;
using std::endl;


typedef char Yes_t;
struct No_t { char more_than_one[4]; };


template<typename TY>
struct _can_convertToString
  {
    static TY & probe();
    
    static Yes_t check(string);
    static No_t  check(...);
    
  public:
    static const bool value = (sizeof(Yes_t)==sizeof(check(probe())));
  };


class SubString
  : public string
  { 
  public:
      SubString() : string("sublunar") { }
  };

class Something { };

struct SomehowStringy
  {
    operator string() { return "No such thing"; }
  };

struct SomehowSub
  {
    operator SubString() { return SubString(); }
  };

class SomehowSubSub
  : public SomehowSub
  { 
  };




template<typename TY>
bool
investigate (TY const&)
  {
    return _can_convertToString<TY>::value;
  }

#define SHOW_CHECK(_EXPR_) cout << STRINGIFY(_EXPR_) << "\t : " << (investigate(_EXPR_)? "Yes":"No") << endl;

int 
main (int, char**)
  {
    
    SHOW_CHECK (string("nebbich"));
    SHOW_CHECK ("gurks");
    SHOW_CHECK (23.34);
    SHOW_CHECK (23);
    
    string urgs("urgs");
    string & urgs_ref (urgs);
    string const& urgs_const_ref (urgs);
    string * urgs_ptr = &urgs;
    
    SHOW_CHECK (urgs_ref);
    SHOW_CHECK (urgs_const_ref);
    SHOW_CHECK (*urgs_ptr);
    
    SubString sub;
    Something thing;
    const SomehowStringy stringy = SomehowStringy();
    SomehowSubSub subsub;
    SubString const& subRef(subsub);
    
    SHOW_CHECK (sub);
    SHOW_CHECK (thing);
    SHOW_CHECK (stringy);
    SHOW_CHECK (subsub);
    SHOW_CHECK (subRef);
    
    cout <<  "\n.gulp.\n";
    
    return 0;
  }
