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
// 06/19 - use a statefull counting filter in a treeExplorer pipeline
// 03/20 - investigate type deduction bug with PtrDerefIter


/** @file try.cpp
 * Compiling a seemingly valid iterator pipeline failed, due to type deduction problems.
 * As expected, they originate within PtrDerefIter, which I abused here to dereference
 * an unique_ptr -- which might seem strange, yet is true to the spirit of generic programming.
 * Since I consider this a valid usage, the fix is to add a further specialisation to my
 * hand-written RemovePtr trait template in iter-adapter-ptr-deref.hpp (which also justifies
 * in hindsight to use a hand-written trait right within this header, instead of some library).
 */

typedef unsigned int uint;

namespace std {
template <typename _Tp, typename _Dp>
    class unique_ptr;
}

#include "lib/format-cout.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/util.hpp"
#include "lib/iter-adapter-ptr-deref.hpp"
#include "lib/iter-adapter-stl.hpp"
#include "lib/itertools.hpp"

#include <utility>
#include <string>
#include <vector>
#include <memory>
#include <type_traits>

using std::string;
using std::make_unique;



#define SHOW_TYPE(_TY_) \
    cout << "typeof( " << STRINGIFY(_TY_) << " )= " << lib::meta::typeStr<_TY_>() <<endl;
#define SHOW_EXPR(_XX_) \
    cout << "Probe " << STRINGIFY(_XX_) << " ? = " << _XX_ <<endl;

using PStr = std::unique_ptr<string>;
using Strs = std::vector<PStr>;

constexpr auto elems = [](auto& coll) { return lib::ptrDeref (lib::iter_stl::eachElm (coll)); };

namespace lib {
  
  namespace {
    
    template<typename T, typename D>
    struct RemovePtr<std::unique_ptr<T,D>> { typedef T Type; };
  }
}
  namespace {
    template<class IT>
    inline auto
    max (IT&& elms)
    {
      using Val = std::remove_reference_t<typename IT::value_type>;
      Val res = std::numeric_limits<Val>::min();
      for (auto& elm : std::forward<IT> (elms))
        if (elm > res)
          res = elm;
      return res;
    }
  }

int
main (int, char**)
  {
    Strs ss;
    ss.emplace_back(new string{"li"});
    ss.emplace_back(new string{"la"});
    ss.emplace_back(new string{"lutsch"});
    SHOW_EXPR (ss);
    SHOW_EXPR (elems(ss));
    using ITS = decltype(elems(ss));
    SHOW_TYPE (ITS);
    
//  using ITSR = typename ITS::reference;
//  lib::test::TypeDebugger<ITSR> buggy;
    
    auto dings = elems(ss);
    
    int maxVSize = max (lib::transformIterator(dings,
                                               [](string const& ding)
                                                 {
                                                   return ding.length();
                                                 }));
    SHOW_EXPR (maxVSize);
    
    cout <<  "\n.gulp.\n";
    return 0;
  }
