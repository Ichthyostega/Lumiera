/* try.cpp  -  to try out and experiment with new features....
 *             scons will create the binary bin/try
 */
// 03/26 - runtime index access and iteration over tuple-likes
// 06/25 - provide a concept to accept _tuple-like_ objects
// 06/25 - investigate function type detection of std::bind Binders
// 12/24 - investigate problem when perfect-forwarding into a binder
// 12/24 - investigate overload resolution on a templated function similar to std::get
// 11/24 - how to define a bare object location comparison predicate
// 11/23 - prototype for grouping from iterator


/** @file try.cpp
 * Work out a construction scheme to build an efficient element-accessor to data
 * in a tuple-like with run-time indexing. As an extension, run-time pulled iteration
 * can be established, using the IterExplorer framework.
 * @see lib::meta::FunTrampoline
 * @see lib::meta::TupleIdxAdaptor
 */


#include "lib/format-cout.hpp"
#include "lib/format-util.hpp"
#include "test/test-helper.hpp"
#include "test/diagnostic-output.hpp"
#include "lib/meta/common-result.hpp"
#include "lib/meta/tuple-concept.hpp"
#include "lib/meta/tuple-helper.hpp"
#include "lib/meta/function.hpp"
#include "lib/iter-adapter.hpp"
#include "lib/hetero-data.hpp"
#include "lib/meta/trait.hpp"
#include "lib/util.hpp"

#include <array>

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
    cout << "Tulik!! "<< ::test::showType<X>() <<endl;
    lib::meta::forEachIDX<X> ([](auto i)
                                {
                                  using Elm = std::tuple_element_t<i, X>;
                                  cout <<"  "<<uint(i)<<": "<< ::test::showType<Elm>() <<endl;
                                });
  }

namespace lib {
namespace meta{
  
#if false ///////////////////////////////////////////////////////////////////////////////////////////////////TODO : new generic library functions
  /**
   * Concept to require a generic type sequence `Types<TYPES...>`
   */
  template<class TSEQ>
  concept typeseq = requires (TSEQ seq)
    {
      // TSEQ can invoke a function with a Types<TYPES> subclass
      []<typename...TYPES>(Types<TYPES...>&){/*placeholder*/}(seq);
    };
#endif   ///////////////////////////////////////////////////////////////////////////////////////////////////(End) : new generic library functions
  
  
  template<template<class> class BUILD, typeseq TSEQ>
  class FunTrampoline
    {
      using AllElm  = ElmTypes<TSEQ>;
      using Indices = AllElm::Idx;
      using Builder = BUILD<TSEQ>;
      using Functor = Builder::Functor;
      using Table   = std::array<Functor, AllElm::SIZ>;
      
      template<size_t...idx>
      static constexpr Table
      buildFunctors (std::index_sequence<idx...>)
        {
          return {Builder().template build<idx>()...};
        }
      
      static constexpr Table functors = buildFunctors (Indices{});
      
    public:
      static constexpr size_t size() { return functors.size(); }
      static constexpr size_t memSiz() { return sizeof(functors); }
      
      static Functor const&
      dispatch (size_t idx)
        {
          ENSURE (idx < functors.size());
          return functors[idx];
        }
      
      using ResType = _Fun<Functor>::Ret;
    };
  
  
#if false ///////////////////////////////////////////////////////////////////////////////////////////////////TODO : new generic library functions
  template<typename T>
  using isConst = std::is_const<remove_reference_t<T>>;
  template<typename T>
  using isLRef = std::is_lvalue_reference<T>;
  
  
  template<class SEQ>
  using Any_Const = ElmTypes<SEQ>::template OrAll<isConst>;
  
  template<class SEQ>
  using All_LRef = ElmTypes<SEQ>::template AndAll<isLRef>;
  
  template<class SEQ>
  static constexpr bool Any_Const_v = Any_Const<SEQ>::value;
  
  template<class SEQ>
  static constexpr bool All_LRef_v = All_LRef<SEQ>::value;

  

  
  template<typename X, typename U, typename...TYPES>
  struct ProbeCommonType
    {
      
      template<typename T1, typename T2>
      using Reconciled = decltype(true ? std::declval<T1>() : std::declval<T2>());
      
      
      template<typename RES>
      struct FoundResult
        : std::true_type
        {
          using Type = RES;
        };
      struct NoResultFound
        : std::false_type
        { };
        
      template<class T1
              ,class T2
              ,typename = Reconciled<T1,T2>>
      static FoundResult<Reconciled<T1,T2>>
      check(int);
      
      template<class,class>
      static NoResultFound
      check(...);
      
      using FirstResult = decltype(check<X,U>(0));
      
      template<bool success, typename...TS>
      struct Join
        {
          using Result = FirstResult;
        };
    
      template<typename T1, typename...TS>
      struct Join<true, T1,TS...>
        {
          //wenn RES erfolgreich
          using CT = FirstResult::Type;
          using Result = ProbeCommonType<CT,T1,TS...>::Result;
        };
      
      using Result = Join<FirstResult::value, TYPES...>::Result;
    };
  

  
  template<typename...TYPES>
  struct CommonResult
    : ProbeCommonType<TYPES...>::Result
    { };
  
  template<typename T1>
  struct CommonResult<T1>
    : CommonResult<T1,T1>
    { };
  
  template<>
  struct CommonResult<>
    : std::false_type
    { };
  
  template<typename...TYPES>
  struct CommonResult<Types<TYPES...>>
    : CommonResult<TYPES...>
    { };
  
  template<typename...TYPES>
  using CommonResult_t = CommonResult<TYPES...>::Type;
  
  template<typename...TYPES>
  static constexpr bool has_CommonResult_v = CommonResult<TYPES...>::value;
  
  
  template<typename...TYPES>
  concept reconcilable = requires
    {
      typename CommonResult<TYPES...>::Type;
    };
  
  template<typename...TYPES>
  concept reconcilable_ref = reconcilable<TYPES...>
                         and isLRef_v<CommonResult_t<TYPES...>>;
  
  template<typename...TYPES>
  struct CommonResultTraits
    {
      static_assert (reconcilable<TYPES...>, "unable to reconcile these types");
      
      using CommonVal = std::common_type_t<TYPES...>;
      using ResType   = CommonResult_t<TYPES...>;
      //NOTE: std::common_type decays (strips CV and ref)
      static constexpr bool makeConst = isConst_v<ResType>;
      static constexpr bool exposeRef = isLRef_v<ResType>;
    };
#endif   ///////////////////////////////////////////////////////////////////////////////////////////////////(End) : new generic library functions
  
  
  
  template<class TUP>
  class TupleIndexAdaptor
    : public TUP
    {
      template<typeseq SEQ>
      struct SubscriptBuilder
        {
          using Tuple = RefTraits<TUP>::value_type;
          using AddRefs = ElmTypes<SEQ>::template Apply<std::add_lvalue_reference_t>;
          using ResType = CommonResult_t<AddRefs>;
          using Functor = ResType(*)(Tuple&);
          
          template<size_t idx>
          constexpr auto
          build()
            {
              return [](Tuple& tuple) -> ResType
                      {
                        return getElm<idx> (tuple);
                      };
            }
        };
      
      using TypeSeq = ElmTypes<TUP>::Seq;
      using Accessor = FunTrampoline<SubscriptBuilder, TypeSeq>;
    public:
      using ResType  = Accessor::ResType;
      using reference = ResType;
      using value_Type = RefTraits<ResType>::value_type;
      
      template<typename T>
      TupleIndexAdaptor(T&& dataSrc)
        : TUP{forward<T> (dataSrc)}
        { }
      
      using TUP::TUP;
      
      static constexpr size_t size() { return Accessor::size(); }
      
      ResType
      operator[] (size_t idx)
        {
          return Accessor::dispatch(idx)(*this);
        }
    };
  
  template<class TUP>
  TupleIndexAdaptor(TUP) -> TupleIndexAdaptor<std::decay_t<TUP>>;
}}

using namespace lib::meta;

template<typeseq SEQ>
struct Stringify
  {
    using Tuple = ElmTypes<SEQ>::Tup;
    using Functor = string(*)(Tuple const&);
    
    template<size_t idx>
    constexpr auto
    build()
      {
        using ElmT = std::tuple_element_t<idx, Tuple>;
        
        return [](Tuple const& tuple)
                {
                  return util::toString<ElmT> (getElm<idx> (tuple));
                };
      }
    
  };


struct Uber { };
struct Untr : Uber { };
struct Unta : Uber { };


int
main (int, char**)
  {
    using Tup = std::tuple<long,short>;
    using Arr = std::array<int,3>;
    using Hetero = lib::HeteroData<int,string>::Chain<short>::ChainExtent<bool,lib::meta::Nil>::ChainType;
    
    SHOW_EXPR((lib::meta::tuple_like<Tup> ))
    SHOW_EXPR((lib::meta::tuple_like<Arr> ))
    SHOW_EXPR((lib::meta::tuple_like<Hetero> ))
    SHOW_EXPR((lib::meta::tuple_like<string> ))
    SHOW_EXPR((lib::meta::tuple_like<int> ))
    
    show<Tup>();
    show<Arr>();
    show<Hetero>();
    show<int>();
    
    using TupStringy = FunTrampoline<Stringify, ElmTypes<Tup>::Seq>;
SHOW_EXPR(TupStringy::size())
    
    Tup tup23{2,3};
    Tup tup42{4,2};
SHOW_TYPE(decltype(std::get<1>(tup42)))
SHOW_EXPR(std::get<1>(tup42))
SHOW_EXPR(TupStringy::dispatch(0)(tup23))
SHOW_EXPR(TupStringy::dispatch(0)(tup42))
SHOW_EXPR(TupStringy::dispatch(1)(tup23))
SHOW_EXPR(TupStringy::dispatch(1)(tup42))

    using T1 = Types<int const&,long&, char&>;
    using T2 = Types<int*,void*>;
    using T3 = Types<int,string>;
SHOW_EXPR((has_CommonResult_v<T1>))
SHOW_EXPR((has_CommonResult_v<T2>))
SHOW_EXPR((has_CommonResult_v<T3>))
SHOW_EXPR((has_CommonResult_v<>))
SHOW_EXPR((has_CommonResult_v<float*>))
SHOW_EXPR((has_CommonResult_v<float*,void* const>))
  
SHOW_TYPE(CommonResult_t<T1>)
SHOW_TYPE(CommonResult_t<T2>)
SHOW_TYPE(CommonResult_t<float*>)
using CoTx = CommonResult_t<float*,float const*>;
SHOW_TYPE(CoTx)


    TupleIndexAdaptor<Tup> idxAdapt{tup42};
SHOW_EXPR(sizeof(tup42))
SHOW_EXPR(sizeof(idxAdapt))
SHOW_TYPE(TupleIndexAdaptor<Tup>::ResType)
SHOW_EXPR(idxAdapt.size())
SHOW_EXPR(idxAdapt[0])
SHOW_EXPR(idxAdapt[1])
SHOW_EXPR(getElm<0>(static_cast<Tup&> (idxAdapt)))
SHOW_EXPR(getElm<1>(static_cast<Tup&> (idxAdapt)))
SHOW_TYPE(decltype(idxAdapt[1]))

    using Tup2 = std::tuple<int,int&, int>;
    int ii{23};
    Tup2 tup21{-1,ii,55};
    TupleIndexAdaptor idxApt2{tup21};
SHOW_TYPE(decltype(tup21))
    using IdxApt2 = decltype(idxApt2);
SHOW_TYPE(IdxApt2)
SHOW_TYPE(IdxApt2::ResType)
SHOW_EXPR(idxApt2[0])
SHOW_EXPR(idxApt2[1])
SHOW_EXPR(idxApt2[2])
SHOW_EXPR(++idxApt2[0])
SHOW_EXPR(idxApt2[1]++)
SHOW_EXPR(++++idxApt2[2])
SHOW_EXPR(tup21)
SHOW_EXPR(Tup2(idxApt2))
SHOW_EXPR(ii)

    using TupIt = lib::IterableDecorator<lib::IdxStoreCore<IdxApt2>>;
    TupIt tupIt{move(idxApt2)};
SHOW_EXPR( bool(tupIt) )
SHOW_EXPR( *tupIt )
SHOW_TYPE(decltype(*tupIt))
    ++*tupIt;
    --ii;
SHOW_EXPR( *tupIt )
SHOW_EXPR(util::join(tupIt, "▶▶"))

SHOW_EXPR(( reconcilable<long,string> ))
SHOW_EXPR(( reconcilable<long&,string&> ))
SHOW_EXPR(( reconcilable<long,short> ))
SHOW_EXPR(( reconcilable<long&,short&> ))
SHOW_EXPR(( reconcilable_ref<long,string> ))
SHOW_EXPR(( reconcilable_ref<long&,string&> ))
SHOW_EXPR(( reconcilable_ref<long,short> ))
SHOW_EXPR(( reconcilable_ref<long&,const long&> ))
    using TX = CommonResult_t<long&,const long&>;
SHOW_TYPE(TX)
SHOW_EXPR(( reconcilable<float*,void*> ))
SHOW_EXPR(( reconcilable<void,void*> ))
SHOW_EXPR(( reconcilable<float*&,void*&> ))
    using TX2 = CommonResult_t<float*&,float*&>;
SHOW_TYPE(TX2)
SHOW_EXPR(( reconcilable<Uber,Unta> ))
SHOW_EXPR(( reconcilable<Untr,Unta> ))
SHOW_EXPR(( reconcilable<Untr&,Unta&> ))
SHOW_EXPR(( reconcilable<Uber&,Unta&> ))
SHOW_EXPR(( reconcilable_ref<Uber&,Unta&> ))
SHOW_EXPR(( reconcilable_ref<Uber&,Unta> ))
SHOW_EXPR(( reconcilable<Uber&,Unta> ))
SHOW_EXPR(( reconcilable<Uber,Untr&> ))
    using TX3 = CommonResult_t<Uber&,Unta>;
SHOW_TYPE(TX3)
    using TX4 = CommonResult_t<Unta&, const Uber&>;
SHOW_TYPE(TX4)
SHOW_EXPR(( reconcilable<Uber&,Unta&,Untr&> ))
    using TX5 = CommonResult_t<const Uber&,Unta&,Untr&>;
SHOW_TYPE(TX5)
    using TX6 = CommonResult_t<const int&,uint16_t,int64_t>;
SHOW_TYPE(TX6)
SHOW_EXPR(( CommonResult<int,string>::value ))
SHOW_EXPR(( CommonResult<int,string,long>::value ))
SHOW_EXPR(( CommonResult<const int,int&>::value ))
SHOW_EXPR(( reconcilable<const int&,uint16_t,int64_t> ))
SHOW_EXPR(( reconcilable<const Uber&,Unta&,Untr&> ))
SHOW_EXPR(( reconcilable<Uber&,const Unta&,Untr&> ))
    
    cout <<  "\n.gulp." <<endl;
    return 0;
  }
