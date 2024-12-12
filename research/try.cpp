/* try.cpp  -  to try out and experiment with new features....
 *             scons will create the binary bin/try
 */
// 12/24 - investigate overload resolution on a templated function similar to std::get
// 11/24 - how to define a bare object location comparison predicate
// 11/23 - prototype for grouping from iterator


/** @file try.cpp
 * Find out about the conditions when an overload of a function template is picked.
 * This is an investigation regarding the proper way to overload std::get
 * especially when the base class of the custom type itself is a tuple.
 */

typedef unsigned int uint;


#include "lib/format-cout.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/test/diagnostic-output.hpp"
#include "lib/util.hpp"

#include <utility>
#include <string>
#include <tuple>

using lib::test::showType;

template<typename...TS>
string
showTypes()
{
  return "<" + ((showType<TS>()+",") + ... + ">");
}

using std::tuple;

struct B { };

struct D1 : B { };

struct D2 : D1 { };

string getty (B&)  { return "getty-B&"; }
string getty (D1&&){ return "getty-D1&&"; }
string getty (D1&) { return "getty-D1&"; }



template<class...TS>
string getty (tuple<TS...>&) { return "getty-tuple& "+showTypes<TS...>(); }


template<class...TS>
struct F : tuple<TS...> { };

template<class...TS>
struct FD1 : F<TS...> {};

template<class...TS>
struct FD2 : FD1<TS...> {};


template<class...TS>
string getty (FD1<TS...>&) { return "getty-FD1& "+showTypes<TS...>(); }

int
main (int, char**)
  {
    D2 d2;
    SHOW_EXPR(getty(d2));
    
    FD2<int, char**> fd2;
    SHOW_EXPR(getty(fd2));
    
    cout <<  "\n.gulp.\n";
    return 0;
  }
