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
// 10/18 - investigate insidious reinterpret cast
// 12/18 - investigate the trinomial random number algorithm from the C standard lib
// 04/19 - forwarding tuple element(s) to function invocation


/** @file try.cpp
 * Research how to apply a tuple to a varargs function forwarder.
 * The recent stadard library has a std::apply, which we can not yet use, unfortunately.
 */

typedef unsigned int uint;

#include "lib/format-cout.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/util.hpp"

#include "lib/meta/variadic-helper.hpp"

#include <utility>
#include <string>
#include <tuple>

using std::string;
using std::tuple;



#define SHOW_TYPE(_TY_) \
    cout << "typeof( " << STRINGIFY(_TY_) << " )= " << lib::meta::typeStr<_TY_>() <<endl;
#define SHOW_EXPR(_XX_) \
    cout << "Probe " << STRINGIFY(_XX_) << " ? = " << _XX_ <<endl;

template<typename FUN, typename...ARGS>
void
forwardInvoker (FUN fun, ARGS&&... args)
{
  cout << "forwardInvoker...\n"
       << lib::test::showVariadicTypes(args...)
       << endl;
  fun (std::forward<ARGS>(args)...);
}

template<typename FUN, class TUP, size_t...idx>
void
unpack_and_forward (FUN&& fun, TUP& tup, lib::meta::IndexSeq<idx...>)
{
  cout << "unpack_and_forward...\n";
  SHOW_TYPE (TUP)
  
  forwardInvoker (std::forward<FUN>(fun), std::get<idx> (std::forward<TUP>(tup))...);
}

template<typename FUN, typename...ARGS>
void
applyTuple (FUN&& fun, tuple<ARGS...>& args)
{
  using Tup = tuple<ARGS...>;
  cout << "applyTuple...\n";
  SHOW_TYPE (Tup)
  
  using SequenceIterator = typename lib::meta::BuildIdxIter<ARGS...>::Ascending;
  
  unpack_and_forward (std::forward<FUN>(fun), args, SequenceIterator());
}




int
main (int, char**)
  {
    auto tup = std::make_tuple(1,2,3u);
    auto fun = [](int a, int b, int c)
                  {
                    cout << a<<"+"<<b<<"+"<<c<<"="<<(a+b+c)<<endl;
                  };
    
    applyTuple (fun, tup);
    
    cout <<  "\n.gulp.\n";
    return 0;
  }
