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


/** @file try.cpp
 ** Metaprogramming: how to detect that a type in question exposes a free function extension point.
 ** Since such an extension point is typically injected alongside with the type exposing the extension
 ** point and intended to be picked up by ADL, all we have to check is if it is valid to invoke the
 ** extension point function with an instance of the target type.
 ** 
 ** There are two difficulties to overcome, though
 ** - a function might return void. And while we may indeed pick up `void` from `decltype(expr)`,
 **   there is not much we can do with a void type. The remedy is just to use this type as template
 **   parameter on another template instantiation, which fails if this type can not legally be formed.
 ** - we do not know how to get a value of the type to probe, in order to feed it into the extension
 **   point function. Fortunately, the `std::declval<TYPE>()` function was included into the C++ language
 **   for this very purpose.
 */

typedef unsigned int uint;

#include "lib/format-cout.hpp"
#include "lib/format-util.hpp"
#include "lib/meta/duck-detector.hpp"
#include "lib/test/test-helper.hpp"

#include <functional>
#include <utility>
#include <string>

using lib::meta::No_t;
using lib::meta::Yes_t;
using lib::test::showSizeof;

using std::function;
using std::forward;
using std::move;
using std::string;




#define SHOW_TYPE(_TY_) \
    cout << "typeof( " << STRINGIFY(_TY_) << " )= " << lib::meta::typeStr<_TY_>() <<endl;
#define SHOW_EXPR(_XX_) \
    cout << "Probe " << STRINGIFY(_XX_) << " ? = " << _XX_ <<endl;


void
fun1 (long)
  {
    cout << "long FUN" <<endl;
  }

int
fun1 (string, long)
  {
    cout << "string FUN" <<endl;
    return 12;
  }

void
fun1 ()
  {
    cout << "NO FUN" <<endl;
  }


class Cheesy
  { };

class Fishy
  {
    friend void fun1 (Fishy&);
  };

#define META_DETECT_EXTENSION_POINT(_FUN_)                 \
    template<typename TY>                                   \
    class HasExtensionPoint_##_FUN_                          \
      {                                                       \
        template<typename X,                                   \
                 typename SEL = decltype( _FUN_(std::declval<X>()))>\
        struct Probe                                             \
          { };                                                    \
                                                                   \
        template<class X>                                           \
        static Yes_t check(Probe<X> * );                             \
        template<class>                                               \
        static No_t  check(...);                                       \
                                                                        \
      public:                                                            \
        static const bool value = (sizeof(Yes_t)==sizeof(check<TY>(0)));  \
      };


META_DETECT_EXTENSION_POINT (fun)
META_DETECT_EXTENSION_POINT (fun1)

int
main (int, char**)
  {
    fun1 (23);
    fun1 ();
    
    SHOW_EXPR ( HasExtensionPoint_fun<long>::value );
    SHOW_EXPR ( HasExtensionPoint_fun1<long>::value );
    SHOW_EXPR ( HasExtensionPoint_fun1<long&>::value );
    SHOW_EXPR ( HasExtensionPoint_fun1<long&&>::value );
    SHOW_EXPR ( HasExtensionPoint_fun1<char>::value );
    SHOW_EXPR ( HasExtensionPoint_fun1<char&>::value );
    SHOW_EXPR ( HasExtensionPoint_fun1<char&&>::value );
    SHOW_EXPR ( HasExtensionPoint_fun1<string>::value );
    SHOW_EXPR ( HasExtensionPoint_fun1<void>::value );
    
    SHOW_EXPR ( HasExtensionPoint_fun1<Cheesy>::value );
    SHOW_EXPR ( HasExtensionPoint_fun1<Fishy>::value );
    SHOW_EXPR ( HasExtensionPoint_fun1<Fishy&>::value );
    SHOW_EXPR ( HasExtensionPoint_fun1<Fishy&&>::value );
    SHOW_EXPR ( HasExtensionPoint_fun1<Fishy const&>::value );
    
    cout <<  "\n.gulp.\n";
    
    return 0;
  }
