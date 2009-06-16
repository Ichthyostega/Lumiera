/*
  FUNCTION-CLOSURE.hpp  -  metaprogramming tools for closing a function over given arguments
 
  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>
 
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 
*/


/** @file function-closure.hpp
 ** Partial function application and building a complete function closure.
 ** This is a small addendum to (and thin wrapper for) tr1/functional, supporting
 ** the case when a function should be closed over (all) arguments, where especially
 ** the parameter values to close on are provided as a tuple. 
 ** 
 ** @see control::CommandDef usage example
 ** @see function.hpp
 ** @see tuple.hpp
 ** 
 */


#ifndef LUMIERA_META_FUNCTION_CLOSURE_H
#define LUMIERA_META_FUNCTION_CLOSURE_H

#include "lib/meta/typelist.hpp"   /////////////TODO
#include "lib/meta/generator.hpp"  /////////////TODO
#include "lib/meta/function.hpp"

#include <tr1/functional>



namespace lumiera {
namespace typelist{

  using std::tr1::bind;
  //using std::tr1::placeholders::_1;
  //using std::tr1::placeholders::_2;
  using std::tr1::function;


  template< typename SIG>
  struct FunctionSignature;
  
  template< typename RET>
  struct FunctionSignature< function<RET(void)> >
  {
    typedef RET Ret;
    typedef Types<> Args;
  };
  
  template< typename RET
          , typename A1
          >
  struct FunctionSignature< function<RET(A1)> >
  {
    typedef RET Ret;
    typedef Types<A1> Args;
  };
  
  template< typename RET
          , typename A1
          , typename A2
          >
  struct FunctionSignature< function<RET(A1,A2)> >
  {
    typedef RET Ret;
    typedef Types<A1,A2> Args;
  };
  
  template< typename RET
          , typename A1
          , typename A2
          , typename A3
          >
  struct FunctionSignature< function<RET(A1,A2,A3)> >
  {
    typedef RET Ret;
    typedef Types<A1,A2,A3> Args;
  };
  
  template< typename RET
          , typename A1
          , typename A2
          , typename A3
          , typename A4
          >
  struct FunctionSignature< function<RET(A1,A2,A3,A4)> >
  {
    typedef RET Ret;
    typedef Types<A1,A2,A3,A4> Args;
  };
  
  template< typename RET
          , typename A1
          , typename A2
          , typename A3
          , typename A4
          , typename A5
          >
  struct FunctionSignature< function<RET(A1,A2,A3,A4,A5)> >
  {
    typedef RET Ret;
    typedef Types<A1,A2,A3,A4,A5> Args;
  };
  
  
  template<typename RET, typename LI>
  struct FunctionTypedef;
  
  template< typename RET>
  struct FunctionTypedef<RET, Types<> >
  {
    typedef function<RET(void)> Func;
    typedef          RET Sig();
  };
  
  template< typename RET
          , typename A1
          >
  struct FunctionTypedef<RET, Types<A1> >
  {
    typedef function<RET(A1)> Func;
    typedef          RET Sig(A1);
  };
  
  template< typename RET
          , typename A1
          , typename A2
          >
  struct FunctionTypedef<RET, Types<A1,A2> >
  {
    typedef function<RET(A1,A2)> Func;
    typedef          RET Sig(A1,A2);
  };
  
  template< typename RET
          , typename A1
          , typename A2
          , typename A3
          >
  struct FunctionTypedef<RET, Types<A1,A2,A3> >
  {
    typedef function<RET(A1,A2,A3)> Func;
    typedef          RET Sig(A1,A2,A3);
  };
  
  template< typename RET
          , typename A1
          , typename A2
          , typename A3
          , typename A4
          >
  struct FunctionTypedef<RET, Types<A1,A2,A3,A4> >
  {
    typedef function<RET(A1,A2,A3,A4)> Func;
    typedef          RET Sig(A1,A2,A3,A4);
  };
  
  template< typename RET
          , typename A1
          , typename A2
          , typename A3
          , typename A4
          , typename A5
          >
  struct FunctionTypedef<RET, Types<A1,A2,A3,A4,A5> >
  {
    typedef function<RET(A1,A2,A3,A4,A5)> Func;
    typedef          RET Sig(A1,A2,A3,A4,A5);
  };
  
  
  
  /////////////////////////very basic facility: Typed tuples
  
  template<class T, class TYPES>
  struct Prepend;
  
  template< typename A1
          , typename A2
          , typename A3
          , typename A4
          , typename A5
          , typename IGN
          >
  struct Prepend<A1, Types<A2,A3,A4,A5,IGN> >
  {
    typedef Types<A1,A2,A3,A4,A5> Tuple;
  };

  template<class TYPES>
  struct Tuple;

  template<>
  struct Tuple<NullType>
    {
      typedef NullType HeadType;
      typedef Types<>  TailType;
      typedef Types<>  Type;
      
      typedef NullType ArgList_;
      typedef Tuple<Type> ThisTuple;
      typedef Tuple<NullType> Tail;
      enum { SIZE = 0 };
      
      NullType getHead() { return NullType(); }
      Tail&    getTail() { return *this;      }
      
      Tuple (HeadType const&, Tail const&) { }
      Tuple ()                             { }
    };
  
  template<class TY, class TYPES>
  struct Tuple<Node<TY,TYPES> >
    : Tuple<TYPES>
    {
      typedef TY                                   HeadType;
      typedef typename Tuple<TYPES>::Type          TailType;
      typedef typename Prepend<TY,TailType>::Tuple Type;
      
      typedef Node<TY,TYPES> ArgList_;
      typedef Tuple<Type> ThisTuple;
      typedef Tuple<TYPES> Tail;
      enum { SIZE = count<ArgList_>::value };
      
      Tuple ( TY a1 =TY()
            , Tail tail =Tail()
            )
        : Tuple<TYPES> (tail.getHead(), tail.getTail()),
          val_(a1)
        { }
      
      TY  & getHead() { return val_; }
      Tail& getTail() { return static_cast<Tail&> (*this); }
      
    private:
      TY val_;
    };
  
  ////TODO move in sub-scope
  template<class TUP,uint i>
  struct Shifted
    {
      typedef typename TUP::Tail Tail;
      typedef typename Shifted<Tail,i-1>::TupleType TupleType;
    };
  template<class TUP>
  struct Shifted<TUP,0>
    { 
      typedef Tuple<typename TUP::ArgList_> TupleType;
    };
      
  
  template< typename T1
          , typename T2
          , typename T3
          , typename T4
          , typename T5
          >
  struct Tuple<Types<T1,T2,T3,T4,T5> >
    : Tuple<typename Types<T1,T2,T3,T4,T5>::List>
    {
      typedef T1                          HeadType;
      typedef Types<T2,T3,T4,T5,NullType> TailType;
      typedef Types<T1,T2,T3,T4,T5>       Type;
      
      typedef typename Type::List ArgList_;
      typedef Tuple<Type> ThisTuple;
      typedef Tuple<TailType> Tail;
      enum { SIZE = count<ArgList_>::value };
      
      Tuple ( T1 a1 =T1()
            , T2 a2 =T2()
            , T3 a3 =T3()
            , T4 a4 =T4()
            , T5 a5 =T5()
            )
        : Tuple<ArgList_>(a1, Tuple<TailType>(a2,a3,a4,a5))
        { }
      
      using Tuple<ArgList_>::getHead;
      using Tuple<ArgList_>::getTail;
      
      template<uint i>
      typename Shifted<ThisTuple,i>::TupleType&
      getShifted ()
        {
          typedef typename Shifted<ThisTuple,i>::TupleType Tail_I;
          return static_cast<Tail_I&> (*this);
        }
      
      template<uint i>
      typename Shifted<ThisTuple,i>::TupleType::HeadType&
      getAt ()
        {
          return getShifted<i>().getHead();
        }
      
      NullType&
      getNull()
        {
          static NullType nix;
          return nix; 
        }
    };
  
  
  /**
   * Decorating a tuple type with auxiliary data access operations.
   * This helper template builds up a subclass of the given BASE type
   * (which is assumed to be a Tuple or at least need to be copy constructible
   * from \c Tuple<TYPES> ). The purpose is to use the Tuple as storage, but
   * to add a layer of access functions, which in turn might rely on the exact
   * type of the individual elements within the Tuple. To achieve this, for each
   * type within the Tuple, the BASE type is decorated with an instance of the
   * template passed in as template template parameter _X_. Each of these
   * decorating instances is provided with a member pointer to access "his"
   * specific element within the underlying tuple.
   * 
   * The decorating template _X_ need to take its own base class as template
   * parameter. Typically, operations on _X_ will be defined in a recursive fashion,
   * calling down into this templated base class. To support this, an instantiation
   * of _X_ with the 0 member ptr is generated for detecting recursion end
   * (built as innermost decorator, i.e. immediate subclass of BASE) 
   */
  template
    < typename TYPES
    , template<class,class,uint> class _X_
    , class BASE =Tuple<TYPES>
    , uint i = 0
    >
  class BuildTupleAccessor
    {
      typedef Tuple<TYPES> ArgTuple;
      typedef typename ArgTuple::HeadType Head;
      typedef typename ArgTuple::TailType Tail;
//    typedef Head ArgTuple::*getElm();
      typedef BuildTupleAccessor<Tail,_X_,BASE, i+1> NextBuilder;
      typedef typename NextBuilder::Accessor NextAccessor;
      
      ArgTuple& argData_;
      
    public:
      
      /** type of the product created by this template.
       *  Will be a subclass of BASE */
      typedef _X_<Head, NextAccessor, i> Accessor;
      
      BuildTupleAccessor (ArgTuple& tup)
        : argData_(tup)
        { }
      
      operator Accessor() { return Accessor(argData_); }
      
    };

  template
    < class BASE
    , template<class,class,uint> class _X_
    , uint i
    >
  class BuildTupleAccessor<Types<>, _X_, BASE, i>
    {
      typedef Tuple<Types<> > ArgTuple;
//    typedef NullType BASE::*getElm();
      
    public:
      typedef _X_<NullType, BASE, 0> Accessor;
    };
  
  
  ///////////////////////// creating functional closures
  
  namespace tuple {
      template<uint n>
      struct Apply;
      
      template<>       
      struct Apply<1>
        {
          template<class FUN, typename RET, class TUP>
          static RET
          invoke (FUN f, TUP & arg)
            {
              return f (arg.template getAt<1>()); 
            }
          
          template<class FUN, typename RET, class TUP>
          static RET
          bind (FUN f, TUP & arg)
            {
              return std::tr1::bind (f, arg.template getAt<1>()); 
            }
        };
      
      template<>       
      struct Apply<2>
        {
          template<class FUN, typename RET, class TUP>
          static RET
          invoke (FUN f, TUP & arg)
            {
              return f ( arg.template getAt<1>()
                       , arg.template getAt<2>()
                       ); 
            }
          
          template<class FUN, typename RET, class TUP>
          static RET
          bind (FUN f, TUP & arg)
            {
              return std::tr1::bind (f, arg.template getAt<1>() 
                                      , arg.template getAt<2>()
                                     );
            }
        };
  } // (END) sub-namespace 
  
  template<typename SIG>
  class TupleApplicator
    {
      typedef typename FunctionSignature< function<SIG> >::Args Args;
      typedef typename FunctionSignature< function<SIG> >::Ret  Ret;
      
      enum { ARG_CNT = count<typename Args::List>::value };
      
      
      /** storing a ref to the parameter tuple */
      Tuple<Args>& params_;
      
    public:
      TupleApplicator (Tuple<Args>& args)
        : params_(args)
        { }
      
      function<SIG>  bind (SIG& f)                 { return tuple::Apply<ARG_CNT>::bind (f, params_); }
      function<SIG>  bind (function<SIG> const& f) { return tuple::Apply<ARG_CNT>::bind (f, params_); }
      
      Ret      operator() (SIG& f)                 { return tuple::Apply<ARG_CNT>::invoke (f, params_); }
      Ret      operator() (function<SIG> const& f) { return tuple::Apply<ARG_CNT>::invoke (f, params_); }
    };
  
  
  /**
   * Closing a function over its arguments.
   * This is a small usage example or spin-off,
   * having almost the same effect than invoking tr1::bind.
   * The notable difference is that the function arguments for
   * creating the closure are passed in as one compound tuple.
   */
  template<typename SIG>
  class FunctionClosure
    {
      typedef typename FunctionSignature< function<SIG> >::Args Args;
      typedef typename FunctionSignature< function<SIG> >::Ret  Ret;
      
      function<Ret(void)> closure_;
      
    public:
      FunctionClosure (SIG& f, Tuple<Args>& arg)
        : closure_(TupleApplicator<SIG>(arg).bind(f))
        { }
      FunctionClosure (function<SIG> const& f, Tuple<Args>& arg)
        : closure_(TupleApplicator<SIG>(arg).bind(f))
        { }
      
      Ret operator() () { return closure_(); }
    };
  
  
/*
  template<typename TYPES>
  struct BuildClosure
    : InstantiateWithIndex<TYPES, Accessor, I>
    {
      
    };
*/
  
  ///////////////////////// additional typelist manipulators
  
  template<class TYPES>
  struct SplitLast;
  
  template<>
  struct SplitLast<NullType>
    {
      typedef NullType Type;
      typedef NullType Prefix;
    };
  
  template<class TY>
  struct SplitLast<Node<TY,NullType> >
    {
      typedef TY Type;
      typedef NullType Prefix;
    };
  
  template<class TY, class TYPES>
  struct SplitLast<Node<TY,TYPES> >
    {
      typedef typename SplitLast<TYPES>::Type Type;
      typedef typename Append<TY, typename SplitLast<TYPES>::Prefix>::List Prefix;
    };

  
  
}} // namespace lumiera::typelist
#endif
