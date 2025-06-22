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
 * [Stackoverflow]: https://stackoverflow.com/q/68443804/444796
 */


#include "lib/format-cout.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/test/diagnostic-output.hpp"
#include "lib/meta/tuple-helper.hpp"
#include "lib/hetero-data.hpp"
#include "lib/util.hpp"

#include <concepts>

using std::string;

namespace lib {
namespace meta {
  
  template<class TUP>
  concept tuple_sized = requires
    {
      { std::tuple_size<TUP>::value } -> std::convertible_to<size_t>;
    };
  

  template<class TUP, std::size_t N>
  concept tuple_adl_accessible = requires(TUP tup)
    {
      typename std::tuple_element_t<N, TUP>;
      { get<N>(tup) } -> std::convertible_to<std::tuple_element_t<N, TUP>&>;
    };
    
  template<class TUP, std::size_t N>
  concept tuple_mem_accessible = requires(TUP tup)
    {
      typename std::tuple_element_t<N, TUP>;
      { tup.template get<N>() } -> std::convertible_to<std::tuple_element_t<N, TUP>&>;
    };
    
  template<class TUP, std::size_t N>
  concept tuple_accessible = tuple_mem_accessible<TUP,N> or tuple_adl_accessible<TUP,N>;
  
  template<class TUP>
  class AndAll
    {
      template<size_t...idx>
      static constexpr bool
      canAccessAll (std::index_sequence<idx...>)
        {
          return (tuple_accessible<TUP, idx> and ...);
        }
      
      using IdxSeq = typename ElmTypes<TUP>::Idx;
      
    public:
      static constexpr bool can_AccessElement = canAccessAll(IdxSeq{});
    };
    
    
    template<class TUP>
    concept tuple_like = not is_reference_v<TUP>
                     and tuple_sized<remove_cv_t<TUP>>
                     and AndAll<remove_cv_t<TUP>>::can_AccessElement;
  
}}//namespace lib::meta



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
    cout << "Tup!! "<< lib::test::showType<X>() <<endl;
    lib::meta::forEachIDX<X> ([](auto i)
                                {
                                  using Elm = std::tuple_element_t<i, X>;
                                  cout <<"  "<<uint(i)<<": "<< lib::test::showType<Elm>() <<endl;
                                });
  }


int
main (int, char**)
  {
    using Tup = std::tuple<long>;
    using Arr = std::array<int,3>;
    using Hetero = lib::HeteroData<int,string>::Chain<short>::ChainExtent<bool,lib::meta::Nil>::ChainType;
    
    SHOW_EXPR((lib::meta::tuple_sized<Tup> ))
    SHOW_EXPR((lib::meta::tuple_sized<Arr> ))
    SHOW_EXPR((lib::meta::tuple_sized<Hetero> ))
    SHOW_EXPR((lib::meta::tuple_sized<int> ))
    
    SHOW_EXPR((lib::meta::tuple_accessible<Tup,0>))
//  SHOW_EXPR((lib::meta::tuple_accessible<Tup,2>))
    SHOW_EXPR((lib::meta::tuple_accessible<Hetero,0>))
    SHOW_EXPR((lib::meta::AndAll<Tup>::can_AccessElement))
    SHOW_EXPR((lib::meta::AndAll<Hetero>::can_AccessElement))
    
    SHOW_EXPR((lib::meta::tuple_like<Tup> ))
    SHOW_EXPR((lib::meta::tuple_like<Arr> ))
    SHOW_EXPR((lib::meta::tuple_like<Hetero> ))
    SHOW_EXPR((lib::meta::tuple_like<int> ))
    
    show<Tup>();
    show<Arr>();
    show<Hetero>();
    show<int>();
    
    cout <<  "\n.gulp." <<endl;
    return 0;
  }
