/* try.cpp  -  to try out and experiment with new features....
 *             scons will create the binary bin/try
 */
// 12/24 - investigate overload resolution on a templated function similar to std::get
// 11/24 - how to define a bare object location comparison predicate
// 11/23 - prototype for grouping from iterator


/** @file try.cpp
 * Find out about the conditions when an overload of a function template is picked.
 * This is an investigation regarding the proper way to overload `std::get`
 * especially when the base class of the custom type itself is a tuple.
 * 
 * As it turns out, overload resolution works as expected; rather the implementation
 * of `std::get` causes the problems, as it triggers an assertion immediately when
 * instantiated with out-of-bounds parameters, which prevents the overload resolution
 * to commence and directly terminates the compilation. The reason is that this
 * standard implementation relies on std::tuple_element<I,T> to do the actual
 * bounds checking. This can be demonstrated by extracting the standard
 * implementation and our custom implementation under a different name.
 */

typedef unsigned int uint;


#include "lib/format-cout.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/test/diagnostic-output.hpp"
#include "lib/hetero-data.hpp"
#include "lib/util.hpp"

#include <utility>
#include <string>
#include <tuple>

using lib::test::showTypes;
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


template<class...TS>
string getty (lib::HeteroData<TS...>&) { return "getty-Hetero& "+showTypes<TS...>(); }


  template<std::size_t __i, typename... _Elements>
//  constexpr std::__tuple_element_t<__i, tuple<_Elements...>>&
    decltype(auto)
    gritty(tuple<_Elements...>& __t) noexcept
    { return std::__get_helper<__i>(__t); }

template<size_t I, typename...DATA>
constexpr std::tuple_element_t<I, lib::HeteroData<DATA...>>&
gritty (lib::HeteroData<DATA...> & heDa) noexcept
{
  return heDa.template get<I>();
}


int
main (int, char**)
  {
    D2 d2;
    SHOW_EXPR(getty(d2));
    
    FD2<int, char**> fd2;
    SHOW_EXPR(getty(fd2));
    
    using Het = lib::HeteroData<uint,double>;
    Het h1;
    SHOW_EXPR(getty(h1));
//  SHOW_EXPR(std::get<1>(h1) = 5.5)
    SHOW_EXPR(h1.get<1>() = 5.5)
    
    using Constructor = Het::Chain<bool,string>;
    auto h2 = Constructor::build (true, "Ψ");
    h2.linkInto(h1);
    
    using Het2 = Constructor::ChainType;
    Het2& chain2 = Constructor::recast (h1);
    SHOW_TYPE(Het2)
    SHOW_EXPR(getty(chain2));
//  SHOW_EXPR(std::get<1>(chain2))
//  SHOW_EXPR(std::get<3>(chain2))
    SHOW_EXPR(chain2.get<1>())
    SHOW_EXPR(chain2.get<3>())
    SHOW_EXPR(gritty<1>(chain2))
    SHOW_EXPR(gritty<3>(chain2))
    
    cout <<  "\n.gulp.\n";
    return 0;
  }
