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


/** @file try.cpp
 ** How to build generic string conversion into `ostream::operator<< `.
 **
 */

typedef unsigned int uint;

#include "lib/p.hpp"
//#include "lib/format-util.hpp"
#include "lib/diff/gen-node.hpp"
//#include "lib/util.hpp"

#include "lib/meta/util.hpp"
#include "lib/meta/trait.hpp"

#include <iostream>
//#include <cstdarg>
#include <type_traits>
#include <utility>
#include <string>
//#include <typeinfo>

using lib::diff::GenNode;
using lib::P;
using lib::meta::can_convertToString;

//using util::unConst;
using std::string;
using std::cout;
using std::endl;

class Reticent
  {
    uint neigh_ = 42;
  };

template<typename X, typename...ARGS>
inline P<X>
newP (ARGS&&... ctorArgs)
{
  return P<X>{new X {std::forward<ARGS>(ctorArgs)...}};
}

/////////////////////////////////////////planned for meta/util.hpp
  template <bool B, class T = void>
  struct enable_if_c {
    typedef T type;
  };

  template <class T>
  struct enable_if_c<false, T> {};
  
  template <class Cond, class T = void>
  using enable_if = typename enable_if_c<Cond::value, T>::type;
  
  template <class Cond, class T = void>
  using disable_if = typename enable_if_c<not Cond::value, T>::type;
  
/////////////////////////////////////////planned for meta/util.hpp


///////////////////////////////copied from format-util.hpp
  template<typename TY>
  inline string
  typeStr (const TY* obj=0)
  {
    auto mangledType = obj? typeid(obj).name()
                          : typeid(TY).name();
    return string("«")+ mangledType +"»";
  }
  
  template<typename TY>
  inline string
  typeStr (TY const& ref)
  { return typeStr(&ref); }
  
  
  template<typename X, typename COND =void>
  struct CustomStringConv
    {
      static string invoke (X const& x) { return typeStr(x); }
    };
  
  template<typename X>
  struct CustomStringConv<X,     enable_if<can_convertToString<X>> >
    {
      static string
      invoke (X const& val)
        try        { return string(val); }
        catch(...) { return ""; }
    };
  
///////////////////////////////copied from format-util.hpp


template<typename X>
inline string
stringz (P<X> ptr)
{
  if (not ptr)
    return "P"+typeStr(ptr.get())+"{ null }";
  else
    return CustomStringConv<X>::invoke (*ptr);
}



/////////////////////////////////////////reworked traits
  namespace {
    
    using lib::meta::Strip;
    using std::__or_;
    using std::__and_;
    using std::__not_;
    
    template<typename T, typename U>
    struct is_basically
      : std::is_same <typename Strip<T>::TypePlain
                     ,typename Strip<U>::TypePlain>
      { };
    
    template<typename X>
    struct can_lexical2string
      : __or_< std::is_arithmetic<X>
             , is_basically<X, string>
             , is_basically<typename std::remove_all_extents<X>::type, char>
             >
      { };
  }
/////////////////////////////////////////reworked traits
  
  
  
/////////////////////////////////////////planned new ostream inclusion
  namespace {
    
    template<typename X>
    struct use_StringConversion
      {
        enum { value = can_convertToString<X>::value
                  &&   !can_lexical2string<X>::value
             };
      };
    
    template<typename X>
    struct use_ObjectTypeIndicator
      : __and_<__not_<can_convertToString<X>>
              ,__not_<can_lexical2string<X>>
              ,std::is_object<X>
              >
      { };
  }
  
  template<typename X, typename =  enable_if<use_StringConversion<X>>>
  std::ostream&
  operator<< (std::ostream& os, X const& obj)
  {
    return os << CustomStringConv<X>::invoke (obj);
  }
//
//  template<typename X, typename =  enable_if<use_ObjectTypeIndicator<X>>>
//  std::ostream&
//  operator<< (std::ostream& os, X const& obj)
//  {
//    return os << CustomStringConv<X>::invoke (obj);
//  }

  template<typename X>
  std::ostream&
  operator<< (std::ostream& os, P<X> const& ptr)
  {
    return os << stringz (ptr);
  }

/////////////////////////////////////////planned new ostream inclusion


int
main (int, char**)
  {
    auto psss = newP<Reticent>();
    auto gnng = newP<GenNode>("Hui", "Buh");
    
#define SHOW_CHECK(_EXPR_) cout << STRINGIFY(_EXPR_) << "\t : " << (_EXPR_::value? "Yes":"No") << endl;
    
    using CharLit = typeof("bla");
    
    using BasicallyString = is_basically<CharLit, string>;
    using BasicallyChar = is_basically<std::remove_all_extents<CharLit>::type, char>;
    
    SHOW_CHECK (BasicallyChar);
    SHOW_CHECK (BasicallyString);
    SHOW_CHECK (std::is_arithmetic<CharLit>);
    SHOW_CHECK (can_lexical2string<CharLit>);
    SHOW_CHECK (can_convertToString<CharLit>);
    SHOW_CHECK (use_StringConversion<CharLit>);
    
    cout << "mauu..." << psss <<endl;
    cout << "wauu..." << gnng <<endl;
    
//  cout << "mauuu.." << *psss <<endl;   ///////////does not compile (but error message is misleading)
    cout << "wauuu.." << *gnng <<endl;
    
    cout <<  "\n.gulp.\n";
    
    return 0;
  }
