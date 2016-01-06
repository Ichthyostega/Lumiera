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
 ** This task is actually a conglomerate of several chores:
 ** - sanitise and segregate the type-traits usage
 ** - disentangle the existing toString conversion helper
 ** - extract a basic form from this helper, which can be placed
 **   into a header with minimal dependencies. After some consideration,
 **   I decided to allow `<typeinfo>` in this category, which allows us
 **   at least to show a type name as fallback
 ** - distill an essential version of `enable_if`, which can be inlined.
 **   This allows us to get rid of `boost::enable_if` finally.
 ** - build a sensible `operator string()` for our `lib::P` based on this
 ** - and _finally_, to come up with a templated version of the `ostream`
 **   inserter `operator<<`, which does not cause too much havoc when
 **   used by default. The greatest challenge here is to avoid ambiguous
 **   overloads, yet also to deal with references, `void` and arrays.
 **
 ** \par policy
 ** What shall be expected from such a generic toString conversion?
 ** It should be _minimal_, it should be _transparent_ and it should
 ** always work and deliver a string, irrespective of the circumstances.
 ** By extension, this means that we do not want to differentiate much
 ** between values, references and pointers, which also means, we do
 ** not want to indicate pointers explicitly (just signal NULL, when
 ** encountered). The situation is slightly different for the `ostream`
 ** inserter; in a modern GUI application, there isn't much use for
 ** STDOUT and STDERR, beyond error messages and unit testing.
 ** Thus, we can strive at building a more convenient flavour
 ** here, which does indeed even shows the address of pointers.
 ** 
 */

typedef unsigned int uint;

#include "lib/p.hpp"
#include "lib/diff/gen-node.hpp"

#include "lib/meta/util.hpp"
#include "lib/meta/trait.hpp"

#include <iostream>
#include <type_traits>
#include <utility>
#include <string>

using lib::diff::GenNode;
using lib::P;
using lib::meta::enable_if;
using lib::meta::typeStr;
using lib::meta::can_convertToString;
using lib::meta::CustomStringConv;

using std::string;
using std::cout;
using std::endl;







///////////////////////////////shall go into the implementation of lib::P
template<typename X>
inline string
stringz (P<X> ptr)
{
  if (not ptr)
    return "⟂ P<"+typeStr(ptr.get())+">";
  else
    return CustomStringConv<X>::invoke (*ptr);
}



/////////////////////////////////////////reworked traits
  namespace {
    
    using lib::meta::Strip;
    using std::is_convertible;
    using std::is_arithmetic;
    using std::is_same;
    using std::__not_;
    using std::__and_;
    using std::__or_;
    
    template<typename T, typename U>
    struct is_basically
      : is_same <typename Strip<T>::TypeReferred
                ,typename Strip<U>::TypeReferred>
      { };
    
    template<typename X>
    struct is_StringLike
      : __or_< is_basically<X, string>
             , is_convertible<X, const char*>
             >
      { };
    
    template<typename X>
    struct can_lexical2string
      : __or_< is_arithmetic<X>
             , is_StringLike<X>
             >
      { };
  }
  
namespace lib {
namespace meta {
  template<>
  struct Unwrap<void>
    {
      typedef void Type;
    };
}}
/////////////////////////////////////////reworked traits



/////////////////////////////////////////planned new ostream inclusion
  namespace {
    
    template<typename X>
    struct use_StringConversion4Stream
      : __and_< std::is_class<typename Strip<X>::TypePlain>
              ,__not_<std::is_pointer<X>>
              ,__not_<can_lexical2string<X>>
              >
      { };
  }
  
  template<typename X, typename =  enable_if <use_StringConversion4Stream<X>>>
  std::ostream&
  operator<< (std::ostream& os, X const& obj)
  {
    return os << CustomStringConv<X>::invoke (obj);
  }
  
  template<typename X, typename =  enable_if <use_StringConversion4Stream<X>>>
  std::ostream&
  operator<< (std::ostream& os, X* ptr)
  {
    if (ptr)
      return os << (void*)ptr << " ↗" << *ptr;
    else
      return os << "⟂ «" << typeStr<X>() << "»";
  }
  
  template<typename X>
  std::ostream&
  operator<< (std::ostream& os, P<X> const& ptr)
  {
    return os << stringz (ptr);
  }
  
/////////////////////////////////////////planned new ostream inclusion



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



template<typename T>
using BasicallyString = is_basically<T, string>;
template<typename T>
using BasicallyChar = is_basically<typename std::remove_all_extents<T>::type, char>;


void
showTypes()
  {
    
#define SHOW_CHECK(_EXPR_) cout << STRINGIFY(_EXPR_) << "\t : " << (_EXPR_::value? "Yes":"No") << endl;
#define ANALYSE(_TYPE_)                 \
    cout << "Type: " STRINGIFY(_TYPE_) " ......"<<endl;   \
    SHOW_CHECK (BasicallyChar<_TYPE_>);   \
    SHOW_CHECK (BasicallyString<_TYPE_>);  \
    SHOW_CHECK (std::is_arithmetic<_TYPE_>);\
    SHOW_CHECK (can_lexical2string<_TYPE_>); \
    SHOW_CHECK (can_convertToString<_TYPE_>); \
    SHOW_CHECK (use_StringConversion4Stream<_TYPE_>);
    
    
    using CharLit = typeof("bla");
    using CharPtr = typeof(const char*);
    using GenNodePtr = typeof(GenNode*);
    using GenNodeRef = typeof(GenNode&);
    
    ANALYSE (string);
    ANALYSE (CharLit);
    ANALYSE (CharPtr)
    ANALYSE (Reticent)
    ANALYSE (P<Reticent>)
    ANALYSE (GenNode)
    ANALYSE (GenNodePtr)
    ANALYSE (GenNodeRef)
    ANALYSE (P<GenNode>)
    cout << endl;
  }



int
main (int, char**)
  {
    showTypes();
    
    auto psss = newP<Reticent>();
    auto gnng = newP<GenNode>("Hui", "Buh");
    
    cout << "mauu..." << psss <<endl;
    cout << "wauu..." << gnng <<endl;
    
    cout << "mauuu.." << *psss <<endl;
    cout << "wauuu.." << *gnng <<endl;
    cout << "wauuup." << gnng.get() <<endl;
    
    gnng.reset();
    cout << "aauu..." << gnng       <<endl;
    cout << "aauu..." << gnng.get() <<endl;
    
    cout <<  "\n.gulp.\n";
    
    return 0;
  }
