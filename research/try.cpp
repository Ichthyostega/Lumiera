/* try.cpp  -  to try out and experiment with new features....
 *             scons will create the binary bin/try
 */
// 06/25 - provide a concept to accept _tuple-like_ objects
// 06/25 - investigate function type detection of std::bind Binders
// 12/24 - investigate problem when perfect-forwarding into a binder
// 12/24 - investigate overload resolution on a templated function similar to std::get
// 11/24 - how to define a bare object location comparison predicate
// 11/23 - prototype for grouping from iterator


/** @file try.cpp
 * Develop a concept to detect _tuple-like_ classes, based on the requirements
 * of the »tuple protocol«. Using some ideas from [Stackoverflow] as starting point.
 * However, we model both a _friend function_ `get` and a similar member function
 * as alternatives for element access, and we break down the checks into sub-concepts.
 * [Stackoverflow]: https://stackoverflow.com/q/68443804/444796
 */


#include "lib/format-cout.hpp"
#include "test/test-helper.hpp"
#include "test/diagnostic-output.hpp"
#include "lib/meta/tuple-helper.hpp"
#include "lib/hetero-data.hpp"
#include "lib/util.hpp"

#include <concepts>

using std::string;


template<typename X>
void
show()
  {
    SHOW_TYPE(X)
  }

template<lib::meta::tuple_like X>
void
show()
  {
    cout << "Tup!! "<< ::test::showType<X>() <<endl;
    lib::meta::forEachIDX<X> ([](auto i)
                                {
                                  using Elm = std::tuple_element_t<i, X>;
                                  cout <<"  "<<uint(i)<<": "<< ::test::showType<Elm>() <<endl;
                                });
  }


int
main (int, char**)
  {
    using Tup = std::tuple<long,short>;
    using Arr = std::array<int,3>;
    using Hetero = lib::HeteroData<int,string>::Chain<short>::ChainExtent<bool,lib::meta::Nil>::ChainType;
    
    SHOW_EXPR((lib::meta::tuple_sized<Tup> ))
    SHOW_EXPR((lib::meta::tuple_sized<Arr> ))
    SHOW_EXPR((lib::meta::tuple_sized<Hetero> ))
    SHOW_EXPR((lib::meta::tuple_sized<int> ))
    
    SHOW_EXPR((lib::meta::tuple_element_accessible<Tup,0>))
//  SHOW_EXPR((lib::meta::tuple_element_accessible<Tup,2>))
    SHOW_EXPR((lib::meta::tuple_element_accessible<Hetero,0>))
    SHOW_EXPR((lib::meta::tuple_accessible<Tup>))
    SHOW_EXPR((lib::meta::tuple_accessible<Hetero>))
    
    SHOW_EXPR((lib::meta::tuple_like<Tup> ))
    SHOW_EXPR((lib::meta::tuple_like<Arr> ))
    SHOW_EXPR((lib::meta::tuple_like<Hetero> ))
    SHOW_EXPR((lib::meta::tuple_like<int> ))
    
    show<Tup>();
    show<Arr>();
    show<Hetero>();
    show<int>();
    
    SHOW_EXPR((std::tuple_size_v<const Tup>))
    using Elm1 = std::tuple_element_t<1, const Tup>;
    SHOW_TYPE(Elm1)
    using TupConstSeq = lib::meta::ElmTypes<const Tup>::Seq;
    SHOW_TYPE(TupConstSeq)
    
    using T1 = decltype(lib::meta::getElm<0> (std::declval<Tup>()));
    SHOW_TYPE(T1)
    using T2 = decltype(lib::meta::getElm<0> (std::declval<Tup&>()));
    SHOW_TYPE(T2)
    using T3 = decltype(lib::meta::getElm<0> (std::declval<Tup const&>()));
    SHOW_TYPE(T3)
    
    using H1 = decltype(lib::meta::getElm<4> (std::declval<Hetero>()));
    SHOW_TYPE(H1)
    using H2 = decltype(lib::meta::getElm<4> (std::declval<Hetero&>()));
    SHOW_TYPE(H2)
    using H3 = decltype(lib::meta::getElm<4> (std::declval<Hetero const&>()));
    SHOW_TYPE(H3)
    
    cout <<  "\n.gulp." <<endl;
    return 0;
  }
