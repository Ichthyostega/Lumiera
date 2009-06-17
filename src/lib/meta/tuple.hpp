/*
  TUPLE.hpp  -  metaprogramming utilities for type tuples and data tuples
 
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


/** @file tuple.hpp
 ** Metaprogramming with tuples-of-types and a very simple Tuple datatype.
 ** The metaprogramming part of this header complements typelist.hpp and allows
 ** to re-build a new tuple-of-types from an existing typelist. Such a finite
 ** tuple of types can at times be more handy than a typelist, especially when
 ** capturing specific types to use as template parameter. 
 ** 
 ** Additionally, this header augments the Tuple template into a very simple Tuple
 ** (run time) datatype. This isn't meant as competing with std::tr1::tuple, which is
 ** much more capable, but also has the downside of pulling in a lot of other headers.
 ** But when all we need is to define a generic typed record of N data elements and
 ** later re-accessing them (but no further advanced processing), the Tuple template
 ** might come in handy. 
 ** 
 ** @see control::CommandDef usage example
 ** @see typelist.hpp
 ** @see function.hpp
 ** @see generator.hpp
 ** 
 */


#ifndef LUMIERA_META_TUPLE_H
#define LUMIERA_META_TUPLE_H

#include "lib/meta/typelist.hpp"
#include "lib/meta/typelistutil.hpp"



namespace lumiera {
namespace typelist{


  
  /** 
   * Helper: prepend a type to an existing type sequence,
   * thus shifting all elements within the sequence 
   * to the right, eventually dropping the last element
   */
  template<class T, class TYPES>
  struct Prepend;
  
  template< typename T01
          , typename T02
          , typename T03
          , typename T04
          , typename T05
          , typename T06
          , typename T07
          , typename T08
          , typename T09
          , typename T10
          , typename T11
          , typename T12
          , typename T13
          , typename T14
          , typename T15
          , typename T16
          , typename T17
          , typename T18
          , typename T19
          , typename T20
          , typename IGN
          >
  struct Prepend<T01, Types<     T02,T03,T04,T05
                           , T06,T07,T08,T09,T10
                           , T11,T12,T13,T14,T15
                           , T16,T17,T18,T19,T20
                           , IGN
                           > >
  {
    typedef Types< T01,T02,T03,T04,T05
                 , T06,T07,T08,T09,T10
                 , T11,T12,T13,T14,T15
                 , T16,T17,T18,T19,T20 > Tuple;
  };
  
  
  
  
  /** Helper: separate parts of a type sequence */
  template<class TYPES>
  struct Split;
  
  template< typename T01
          , typename T02
          , typename T03
          , typename T04
          , typename T05
          , typename T06
          , typename T07
          , typename T08
          , typename T09
          , typename T10
          , typename T11
          , typename T12
          , typename T13
          , typename T14
          , typename T15
          , typename T16
          , typename T17
          , typename T18
          , typename T19
          , typename T20
          >
  struct Split<Types< T01,T02,T03,T04,T05
                    , T06,T07,T08,T09,T10
                    , T11,T12,T13,T14,T15
                    , T16,T17,T18,T19,T20
                    > >
  {
    typedef        T01                   Head;
    typedef Types< T01                 > First;
    typedef Types< T01,T02,T03,T04,T05
                 , T06,T07,T08,T09,T10
                 , T11,T12,T13,T14,T15
                 , T16,T17,T18,T19     > Prefix;
    typedef Types<     T02,T03,T04,T05
                 , T06,T07,T08,T09,T10
                 , T11,T12,T13,T14,T15
                 , T16,T17,T18,T19,T20 > Tail;
    typedef Types<                 T20 > Last;
    typedef                        T20   End;
  };
  
  
  
  
  /**
   * Helper: generate a type sequence left shifted 
   * by i steps, filling in NullType at the end
   */  
  template<class TYPES, uint i=1>
  class Shifted
    {
      typedef typename Split<TYPES>::Tail Tail;
    public:
      typedef typename Shifted<Tail,i-1>::Types Types;
      typedef typename Split<Types>::Head       Head;
    };
  
  template<class TYPES>
  struct Shifted<TYPES,0>
    { 
      typedef TYPES Types;
      typedef typename Types::List::Head Head;
    };

  
  /** 
   * simple generic Tuple datatype.
   * Usable both for metaprogramming and as a generic record.
   * The types within this tuple can either be specified
   * as Type sequence or as typelist
   */
  template<class TYPES>
  struct Tuple;
  
  
  template<>
  struct Tuple<NullType>
    {
      typedef NullType HeadType;
      typedef Types<>  TailType;
      typedef Types<>  Type;
      
      typedef NullType        ArgList;
      typedef Tuple<Type>     ThisTuple;
      typedef Tuple<NullType> Tail;
      enum  { SIZE = 0 };
      
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
      
      typedef Node<TY,TYPES> ArgList;
      typedef Tuple<Type>    ThisTuple;
      typedef Tuple<TYPES>   Tail;
      enum  { SIZE = count<ArgList>::value };
      
      Tuple ( TY   a1   =TY()
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
  
  
  template< typename T1
          , typename T2
          , typename T3
          , typename T4
          , typename T5
          , typename T6
          , typename T7
          , typename T8
          , typename T9
          >
  struct Tuple<Types<T1,T2,T3,T4,T5,T6,T7,T8,T9> >
    : Tuple<typename Types<T1,T2,T3,T4,T5,T6,T7,T8,T9>::List>
    {
      typedef       T1                                HeadType;
      typedef Types<T2,T3,T4,T5,T6,T7,T8,T9,NullType> TailType;
      typedef Types<T1,T2,T3,T4,T5,T6,T7,T8,T9>       Type;
      
      typedef typename Type::List ArgList;
      typedef Tuple<Type>         ThisTuple;
      typedef Tuple<TailType>     Tail;
      enum  { SIZE = count<ArgList>::value };
      
      Tuple ( T1 a1 =T1()
            , T2 a2 =T2()
            , T3 a3 =T3()
            , T4 a4 =T4()
            , T5 a5 =T5()
            , T6 a6 =T6()
            , T7 a7 =T7()
            , T8 a8 =T8()
            , T9 a9 =T9()
            )
        : Tuple<ArgList>(a1, Tuple<TailType>(a2,a3,a4,a5,a6,a7,a8,a9))
        { }
      
      using Tuple<ArgList>::getHead;
      using Tuple<ArgList>::getTail;
      
      template<uint i>
      class ShiftedTuple
        { 
          typedef typename Tuple::Type                OurType_;
          typedef typename Shifted<OurType_,i>::Types ShiftedTypes_;
        public:
          typedef Tuple<typename ShiftedTypes_::List> Type;
        };
      
      template<uint i>
      typename ShiftedTuple<i>::Type&
      getShifted ()
        {
          typedef typename ShiftedTuple<i>::Type Tail_I;
          return static_cast<Tail_I&> (*this);
        }
      
      template<uint i>
      typename Shifted<Type,i>::Head&
      getAt ()
        {
          return getShifted<i>().getHead();
        }
    };
  
  
  
  namespace tuple { // some convenience access functions
  
    template<uint n, class TUP> 
    typename TUP::template ShiftedTuple<n>::Type::Head&
    element (TUP& tup)
    {
      return tup.template getAt<n>();
    }
    
    
    
    Tuple<Types<> >
    makeNullTuple ()
    {
      return Tuple<Types<> > ();
    }
    
    
    template< typename T1
            >
    Tuple<Types<T1> >
    make ( T1 a1 =T1()
         )
    {
      return Tuple<Types<T1> > (a1);
    }
    
    
    template< typename T1
            , typename T2
            >
    Tuple<Types<T1,T2> >
    make ( T1 a1 =T1()
         , T2 a2 =T2()
         )
    {
      return Tuple<Types<T1,T2> > (a1,a2);
    }
    
    
    template< typename T1
            , typename T2
            , typename T3
            >
    Tuple<Types<T1,T2,T3> >
    make ( T1 a1 =T1()
         , T2 a2 =T2()
         , T3 a3 =T3()
         )
    {
      return Tuple<Types<T1,T2,T3> > (a1,a2,a3);
    }
    
    
    template< typename T1
            , typename T2
            , typename T3
            , typename T4
            >
    Tuple<Types<T1,T2,T3,T4> >
    make ( T1 a1 =T1()
         , T2 a2 =T2()
         , T3 a3 =T3()
         , T4 a4 =T4()
         )
    {
      return Tuple<Types<T1,T2,T3,T4> > (a1,a2,a3,a4);
    }
    
    
    template< typename T1
            , typename T2
            , typename T3
            , typename T4
            , typename T5
            >
    Tuple<Types<T1,T2,T3,T4,T5> >
    make ( T1 a1 =T1()
         , T2 a2 =T2()
         , T3 a3 =T3()
         , T4 a4 =T4()
         , T5 a5 =T5()
         )
    {
      return Tuple<Types<T1,T2,T3,T4,T5> > (a1,a2,a3,a4,a5);
    }
    
    
    template< typename T1
            , typename T2
            , typename T3
            , typename T4
            , typename T5
            , typename T6
            , typename T7
            , typename T8
            , typename T9
            >
    Tuple<Types<T1,T2,T3,T4,T5,T6,T7,T8,T9> >
    make ( T1 a1 =T1()
         , T2 a2 =T2()
         , T3 a3 =T3()
         , T4 a4 =T4()
         , T5 a5 =T5()
         , T6 a6 =T6()
         , T7 a7 =T7()
         , T8 a8 =T8()
         , T9 a9 =T9()
         )
    {
      return Tuple<Types<T1,T2,T3,T4,T5,T6,T7,T8,T9> > (a1,a2,a3,a4,a5,a6,a7,a8,a9);
    }
    
  }
      

  
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
    < typename TYPES                                ///< Type sequence to use within the Accessor (usually the Tuple Types)
    , template<class,class,class, uint> class _X_   ///< user provided template<Type, Base, TupleType, arg-No>
    , class TUP =Tuple<TYPES>                       ///< the tuple type to build on
    , uint i = 0                                    ///< tuple element index counter
    >
  class BuildTupleAccessor
    {
      typedef Tuple<TYPES> ArgTuple;
      typedef typename ArgTuple::HeadType Head;
      typedef typename ArgTuple::TailType Tail;
      typedef BuildTupleAccessor<Tail,_X_,TUP, i+1> NextBuilder;
      typedef typename NextBuilder::Accessor NextAccessor;
      
      ArgTuple& argData_;
      
    public:
      
      /** type of the product created by this template.
       *  Will be a subclass of TUP */
      typedef _X_< Head            // the type to use for this accessor
                 , NextAccessor    // the base type to inherit from
                 , TUP             // the tuple type we build upon
                 , i               // current element index
                 >    Accessor;
      
      
      BuildTupleAccessor (ArgTuple& tup)
        : argData_(tup)
        { }
      
      /** used to get the product of this builder template... */
      operator Accessor() { return Accessor(argData_); }
      
    };
  
  
  template
    < template<class,class,class, uint> class _X_
    , class TUP
    , uint i
    >
  class BuildTupleAccessor<Types<>, _X_, TUP, i>
    {
      typedef Tuple<Types<> > ArgTuple;
      
    public:
      typedef _X_<NullType, TUP, TUP, 0> Accessor;
    };
  
  

  
  
}} // namespace lumiera::typelist
#endif
