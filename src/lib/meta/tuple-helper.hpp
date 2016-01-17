/*
  TUPLE-HELPER.hpp  -  metaprogramming utilities for type and data tuples

  Copyright (C)         Lumiera.org
    2016,               Hermann Vosseler <Ichthyostega@web.de>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of
  the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/


/** @file tuple.hpp
 ** Metaprogramming with tuples-of-types and the `std::tuple` record.
 ** The metaprogramming part of this header complements typelist.hpp and allows
 ** some additional manipulations on type sequences, especially to integrate
 ** with the Tuples provided by the standard library.
 ** 
 ** @see control::CommandDef usage example
 ** @see TupleHelper_test
 ** @see typelist.hpp
 ** @see function.hpp
 ** @see generator.hpp
 ** 
 */


#ifndef LIB_META_TUPLE_HELPER_H
#define LIB_META_TUPLE_HELPER_H

#include "lib/meta/typelist.hpp"
#include "lib/meta/typelist-util.hpp"
#include "lib/meta/typeseq-util.hpp"
#include "lib/meta/util.hpp"

#include <tuple>


namespace util { // forward declaration
  
  template<typename TY>
  std::string
  toString (TY const& val)  noexcept;
}


namespace lib {
namespace meta {
  
  /**
   * temporary workaround:
   * alternative definition of "type sequence",
   * already using variadic template parameters.
   * @remarks the problem with our existing type sequence type
   *    is that it fills the end of each sequence with NullType,
   *    which was the only way to get a flexible type sequence
   *    prior to C++11. Unfortunately these trailing NullType
   *    entries do not play well with other variadic defs.
   * @deprecated when we switch our primary type sequence type
   *    to variadic parameters, this type will be superfluous.
   */
  template<typename...TYPES>
  struct TySeq
    {
      using Seq = TySeq;
      using List = typename Types<TYPES...>::List;
    };
  
  
  /**
   * temporary workaround: additional specialisation for the template
   * `Prepend` to work also with the (alternative) variadic TySeq.
   * @see typeseq-util.hpp
   */
  template<typename T, typename...TYPES>
  struct Prepend<T, TySeq<TYPES...>>
  {
    using Seq  = TySeq<T, TYPES...>;
    using List = typename Types<T, TYPES...>::List;
  };
  
  
  /**
   * temporary workaround: strip trailing NullType entries from a
   * type sequence, to make it compatible with new-style variadic
   * template definitions.
   * @note the result type is a TySec, to keep it apart from our
   *    legacy (non-variadic) lib::meta::Types
   * @deprecated necessary for the transition to variadic sequences
   */
  template<typename SEQ>
  struct StripNullType;
  
  template<typename T, typename...TYPES>
  struct StripNullType<Types<T,TYPES...>>
    {
      using TailSeq = typename StripNullType<Types<TYPES...>>::Seq;
      
      using Seq = typename Prepend<T, TailSeq>::Seq;
    };
  
  template<typename...TYPES>
  struct StripNullType<Types<NullType, TYPES...>>
    {
      using Seq = TySeq<>;  // NOTE: this causes the result to be a TySeq
    };
  
  
  
  
  namespace { // rebinding helper to create std::tuple from a type sequence
    
    template<typename SEQ>
    struct BuildTupleType
      : std::false_type
      { };
    
    template<typename...TYPES>
    struct BuildTupleType<TySeq<TYPES...>>
      {
        using Type = std::tuple<TYPES...>;
      };
    
    /**
     * temporary workaround: strip trailing NullType entries
     * prior to rebinding to the `std::tuple` type.
     */
    template<typename...TYPES>
    struct BuildTupleType<Types<TYPES...>>
      {
        using VariadicSeq = typename StripNullType<Types<TYPES...>>::Seq;
        
        using Type = typename BuildTupleType<VariadicSeq>::Type;
      };
  }
  
  
  /** Build a `std::tuple` from types given as type sequence
   * @remarks for Lumiera, we deliberately use a dedicated template `Types`
   *    to mark a type sequence of types as such. This allows to pass such a
   *    sequence as first-class citizen. The standard library often (ab)uses
   *    the std::tuple for this purpose, which is an understandable, yet
   *    inferior design choice. We should always favour dedicated types
   *    over clever re-use of existing types.
   */
  template<typename TYPES>
  using Tuple = typename BuildTupleType<TYPES>::Type;
  
  
  using std::tuple_size;
  using std::tuple_element;
  
  
  
  /** match and rebind the type sequence from a tuple */
  template<typename...TYPES>
  struct Types<std::tuple<TYPES...>>
    {
      using Seq  = typename Types<TYPES...>::Seq;
      using List = typename Seq::List;
    };
  
  
  /** trait to detect tuple types */
  template<typename T>
  struct is_Tuple
    : std::false_type
    { };
  
  template<typename...TYPES>
  struct is_Tuple<std::tuple<TYPES...>>
    : std::true_type
    { };
  
  
  
  
  
  /** Hold a sequence of index numbers as template parameters */
  template<size_t...idx>
  struct IndexSeq
    {
      template<size_t i>
      using AppendElm = IndexSeq<idx..., i>;
    };
  
  /** build an `IndexSeq<0, 1, 2, ..., n-1>` */
  template<size_t n>
  struct BuildIndexSeq
    {
      using Ascending = typename BuildIndexSeq<n-1>::Ascending::template AppendElm<n-1>;
      
      template<size_t i>
      using FilledWith = typename BuildIndexSeq<n-1>::template FilledWith<i>::template AppendElm<i>;
    };
  
  template<>
  struct BuildIndexSeq<0>
    {
      using Ascending = IndexSeq<>;
      
      template<size_t>
      using FilledWith = IndexSeq<>;;
    };


#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////OBSOLETE :: TICKET #988
  
  
  /** 
   * simple generic Tuple datatype.
   * Usable both for metaprogramming and as a generic record.
   * The types within this tuple can either be specified
   * as Type sequence or as typelist. Default and canonical
   * is the type-sequence based tuple form \c Tuple<Types<T1,T2,...>>
   * The secondary from of specifying a tuple is based on a typelist
   * (and this form is used actually to implement the storage, while
   * the plain-flat (type sequence based) form acts as interface.
   * 
   * Irrespective of the flavour actually used, you can always
   * - get the canonical TupleType (sequence based)
   * - get the types of head and tail, and a list version of the types
   * - access the head element and the tail tuple
   * - access the Nth element and a shifted-b-N sub (tail) tuple
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
      typedef Tuple<Type>     TupleType;
      typedef Tuple<NullType> ThisType;
      typedef Tuple<NullType> Tail;
      enum  { SIZE = 0 };
      
      NullType& getHead() { return bottom(); }
      Tail&     getTail() { return *this; }
      
      Tuple (HeadType const&, Tail const&) { }
      Tuple ()                             { }
      
      template<uint> struct ShiftedTuple   { typedef Tail Type;};
      template<uint> Tail& getShifted ()   { return *this;     }
      template<uint> NullType&  getAt ()   { return bottom();  }
      
      const NullType getHead_const() const { return bottom();  }
      const Tail&    getTail_const() const { return *this;     }
      
      TupleType& 
      tupleCast ()
        {
          return reinterpret_cast<TupleType&> (*this);
        }
      
      NullType&
      bottom()  const
        {
          return (NullType&) (*this);
        }
    };
  
  
  template<class TY, class TYPES>
  struct Tuple<Node<TY,TYPES> >
    : Tuple<TYPES>
    {
      typedef TY                                 HeadType;
      typedef typename Tuple<TYPES>::Type        TailType;
      typedef typename Prepend<TY,TailType>::Seq Type;
      
      typedef Node<TY,TYPES> ArgList;
      typedef Tuple<Type>    TupleType;
      typedef Tuple<ArgList> ThisType;
      typedef Tuple<TYPES>   Tail;
      enum  { SIZE = count<ArgList>::value };
      
      Tuple ( TY   a1   =TY()
            , Tail tail =Tail()
            )
        : Tail (tail.getHead(), tail.getTail()),
          val_(a1)
        { }
      
      TY  & getHead() { return val_; }
      Tail& getTail() { return static_cast<Tail&> (*this); }
      
      TY   const& getHead_const() const { return val_; }
      Tail const& getTail_const() const { return static_cast<const Tail&> (*this); }
      
      
      template<uint i>
      class ShiftedTuple
        { 
          typedef typename Tuple::Type                OurType_;
          typedef typename Shifted<OurType_,i>::Type  ShiftedTypes_;
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
      
      TupleType& 
      tupleCast () ///< note makes this List-style Tuple appear as plain-flat Tuple
        {
          return reinterpret_cast<TupleType&> (*this);
        }
      
      template<uint i>
      typename Shifted<Type,i>::Head&
      getAt ()
        {
          return getShifted<i>().getHead();
        }
      
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
      typedef Tuple<Type>         TupleType;
      typedef Tuple<Type>         ThisType;
      typedef Tuple<TailType>     Tail;
      enum  { SIZE = count<ArgList>::value };
      
      /** standard ctor: create from values */
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
      
      /** shortcut: allow copy construction from a tuple
       *  which is rather defined by a list type */
      Tuple (Tuple<ArgList> const& listTuple)
        : Tuple<ArgList> (listTuple)
        { }
      
      using Tuple<ArgList>::getHead;
      
      Tail& getTail() ///< note makes the Tail appear as plain-flat shifted tuple
        {
          return Tuple<ArgList>::getTail().tupleCast();
        }
    };
  
  
  template<>
  struct Tuple<Types<> >
    : Tuple<NullType>
    {
      enum  { SIZE = 0 };
      typedef Tuple<NullType> TupNilList;
      typedef Tuple<Types<> > ThisType;
      typedef ThisType        Tail;
      
      
      Tuple ( NullType =NullType()
            , NullType =NullType()
            , NullType =NullType()
            , NullType =NullType()
            , NullType =NullType()
            , NullType =NullType()
            , NullType =NullType()
            , NullType =NullType()
            , NullType =NullType()
            )
        { } ///< end recursion of chained ctor calls
      
      /** shortcut: allow copy construction from a tuple
       *  which is rather defined by a list type */
      Tuple (TupNilList const&)
        { }
    };
  
  
  
  
  /** specialisation to shift plain tuple types */
  template<class TYPES, uint i>
  struct Shifted<Tuple<TYPES>,i>
    { 
      typedef typename Shifted<TYPES,i>::Type Type;
      typedef typename Shifted<TYPES,i>::Head Head;
      typedef Tuple<Type>                     TupleType;
    };
  template<class TYPES>
  struct Shifted<Tuple<TYPES>, 0>
    { 
      typedef typename Tuple<TYPES>::Type     Type;
      typedef typename Tuple<TYPES>::HeadType Head;
      typedef Tuple<Type>                     TupleType;
    };
  
  
  
  /* ====== Helpers for working with Tuples ========= */
  
  namespace tuple { // some convenience access functions
  
    template<uint n, class TUP> 
    inline
    typename Shifted<TUP,n>::Head&
    element (TUP& tup)
    {
      return tup.template getAt<n>();
    }
    
    
    
    inline
    Tuple< Types<> >
    makeNullTuple ()
    {
      return Tuple<Types<> > ();
    }
    
    
    template<typename T1>
    inline
    Tuple< Types<T1> >
    make ( T1 a1
         )
    {
      return Tuple<Types<T1> > (a1);
    }
    
    
    template< typename T1
            , typename T2
            >
    inline
    Tuple< Types<T1,T2> >
    make ( T1 a1
         , T2 a2
         )
    {
      return Tuple<Types<T1,T2> > (a1,a2);
    }
    
    
    template< typename T1
            , typename T2
            , typename T3
            >
    inline
    Tuple< Types<T1,T2,T3> >
    make ( T1 a1
         , T2 a2
         , T3 a3
         )
    {
      return Tuple<Types<T1,T2,T3> > (a1,a2,a3);
    }
    
    
    template< typename T1
            , typename T2
            , typename T3
            , typename T4
            >
    inline
    Tuple< Types<T1,T2,T3,T4> >
    make ( T1 a1
         , T2 a2
         , T3 a3
         , T4 a4
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
    inline
    Tuple< Types<T1,T2,T3,T4,T5> >
    make ( T1 a1
         , T2 a2
         , T3 a3
         , T4 a4
         , T5 a5
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
            >
    inline
    Tuple< Types<T1,T2,T3,T4,T5,T6> >
    make ( T1 a1
         , T2 a2
         , T3 a3
         , T4 a4
         , T5 a5
         , T6 a6
         )
    {
      return Tuple<Types<T1,T2,T3,T4,T5,T6> > (a1,a2,a3,a4,a5,a6);
    }
    
    
    template< typename T1
            , typename T2
            , typename T3
            , typename T4
            , typename T5
            , typename T6
            , typename T7
            >
    inline
    Tuple< Types<T1,T2,T3,T4,T5,T6,T7> >
    make ( T1 a1
         , T2 a2
         , T3 a3
         , T4 a4
         , T5 a5
         , T6 a6
         , T7 a7
         )
    {
      return Tuple<Types<T1,T2,T3,T4,T5,T6,T7> > (a1,a2,a3,a4,a5,a6,a7);
    }
    
    
    template< typename T1
            , typename T2
            , typename T3
            , typename T4
            , typename T5
            , typename T6
            , typename T7
            , typename T8
            >
    inline
    Tuple< Types<T1,T2,T3,T4,T5,T6,T7,T8> >
    make ( T1 a1
         , T2 a2
         , T3 a3
         , T4 a4
         , T5 a5
         , T6 a6
         , T7 a7
         , T8 a8
         )
    {
      return Tuple<Types<T1,T2,T3,T4,T5,T6,T7,T8> > (a1,a2,a3,a4,a5,a6,a7,a8);
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
    inline
    Tuple< Types<T1,T2,T3,T4,T5,T6,T7,T8,T9> >
    make ( T1 a1
         , T2 a2
         , T3 a3
         , T4 a4
         , T5 a5
         , T6 a6
         , T7 a7
         , T8 a8
         , T9 a9
         )
    {
      return Tuple<Types<T1,T2,T3,T4,T5,T6,T7,T8,T9> > (a1,a2,a3,a4,a5,a6,a7,a8,a9);
    }
    
    
    
    /** 
     * Helper to construct a new tuple, partially using provided argument values.
     * Arguments are expected as a tuple, which is assumed to be a sub-tuple of
     * the target type to be created. The start index of this sub-tuple may be
     * provided as additional parameter, otherwise it is assumed to be zero, 
     * (i.e. the sub tuple starting left aligned). Any further arguments
     * of the target type, which aren't covered by the argument tuple,
     * are default initialised. 
     * @param TYPES type sequence or type list denoting the target tuple type
     * @param ARGS  type sequence of type list denoting the argument tuple type
     * @param pos   start index of the ARGS sequence within the TYPES sequence
     * 
     * @note call the embedded #create function to invoke
     * @note when types or positions disagree, argument tuple will be ignored
     * @see TypeTuple_test#check_build_from_subTuple
     */
    template<typename TYPES, typename ARGS, uint pos=0>
    struct BuildTuple
      {
        typedef typename Tuple<TYPES>::TupleType ThisTuple;
        typedef typename Tuple<TYPES>::ArgList TypeList;
        typedef typename Tuple<ARGS>::ArgList ArgTypeList;
        
        /**
         * @param the argument values, contained in a list or flat- tuple
         *          of the type denoted by ARGS
         * @return a plain-flat Tuple<TYPES> instance, initialised with
         *          the values found within arg
         */
        static ThisTuple
        create (Tuple<ArgTypeList> const& arg)
          {
            return BuildTuple<TypeList,ArgTypeList,pos>
                    ::create(arg)
                      .tupleCast();
          }
      };
    
    template<typename TYPES, typename ARGS, uint pos>
    struct BuildTuple<Tuple<TYPES>, Tuple<ARGS>, pos>  ///< tuples allowed instead of plain type sequences/lists
      : BuildTuple<TYPES,ARGS,pos>
      { };
    
    
    template< typename T
            , typename TS
            , typename A
            , typename AS
            , uint pos
            >
    struct BuildTuple<Node<T,TS>, Node<A,AS>, pos>     ///< case: recursion \em before start of arg tuple
      {
        typedef Tuple<Node<T,TS> > ThisTuple;
        typedef Tuple<Node<A,AS> > ThisArg;
        
        static ThisTuple
        create (ThisArg const& arg)
          {
            return ThisTuple( T()
                            , BuildTuple<TS, Node<A,AS>, pos-1>::create(arg)
                            );
          }
      };
    
    
    template< typename A
            , typename TS
            , typename AS
            >
    struct BuildTuple<Node<A,TS>, Node<A,AS>, 0>       ///< case: start of argument tuple detected
      {
        typedef Tuple<Node<A,TS> > ThisTuple;
        typedef Tuple<Node<A,AS> > ThisArg;
        
        static ThisTuple
        create (ThisArg const& arg)
          {
            return ThisTuple( arg.getHead_const()
                            , BuildTuple<TS, AS, 0>::create (arg.getTail_const())
                            );
          }
      };
    
    
    template< typename ARGS
            , uint i
            >
    struct BuildTuple<NullType, ARGS, i>               ///< case: hit end of target typelist
      {
        typedef Tuple<NullType> ThisTuple;
        typedef Tuple<ARGS>     ThisArg;
        
        static ThisTuple
        create (ThisArg const&)
          {
            return ThisTuple();
          }
      };
    
    
    template< typename T
            , typename TS
            , uint i
            >
    struct BuildTuple<Node<T,TS>, NullType, i>         ///< case: hit end of argument tuple
      {
        typedef Tuple<Node<T,TS> > ThisTuple;
        typedef Tuple<NullType>    ThisArg;
        
        static ThisTuple
        create (ThisArg const&)
          {
            return ThisTuple();
          }
      };
    
    
    
  } // (END) access / tuple building helper functions (namespace tuple)
  
  
  
  /** Trait template for detecting a type tuple */
  template<typename TUP>
  class is_Tuple
    {
      template<class X>  struct Check             { typedef No_t It;  };
      template<class TY> struct Check<Tuple<TY> > { typedef Yes_t It; };
      
    public:
      static const bool value = (sizeof(Yes_t)==sizeof(typename Check<TUP>::It));
    };
  
  /** Trait template detecting especially tuples
   *  built directly on top of a Typelist */
  template<typename TUP>
  class is_TupleListType
    {
      template<class X>
      struct Check
        {
          enum{ result = sizeof(No_t)};
        };
      
      template<class N>
      struct Check<Tuple<N> >
      {
        template<class H, class T>
        Yes_t static check(Node<H,T>*);
        Yes_t static check(NullType*);
        No_t  static check(...);
        
        enum{ result = sizeof(check( (N*)0)) };
      };
      
    public: 
      static const bool value = (sizeof(Yes_t)== Check<TUP>::result);
    };
  
  /** Trait template to discern plain tuples and list-type tuples */
  template<typename TUP>
  struct is_TuplePlain
    {
      static const bool value =     is_Tuple<TUP>::value
                                && !is_TupleListType<TUP>::value;
    };
  
  /** Trait template detecting an empty tuple type */
  template<typename TUP>
  class is_NullTuple
    {
      template<class X>
      struct Check
        {
          enum{ result = sizeof(No_t)};
        };
      
      template<class TY>
      struct Check<Tuple<TY> >
      {
        Yes_t static check(Types<>*);
        Yes_t static check(NullType*);
        No_t  static check(...);
        
        enum{ result = sizeof(check( (TY*)0)) };
      };
      
    public: 
      static const bool value = (sizeof(Yes_t)== Check<TUP>::result);
    };
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////OBSOLETE :: TICKET #988
  
  
  /**
   * Decorating a tuple type with auxiliary data access operations.
   * This helper template builds up a subclass of the given TUP (base) type
   * (which is assumed to be a Tuple or at least need to be copy constructible
   * from `Tuple<TYPES>` ). The purpose is to use the Tuple as storage record, but
   * to add a layer of access functions, which in turn might rely on the exact
   * type of the individual elements within the Tuple. To achieve this, for each
   * type within the Tuple, the TUP type is decorated with an instance of the
   * template passed in as template template parameter _X_. Each of these
   * decorating instances is provided with an index number, allowing to
   * access "his" specific element within the underlying tuple.
   * 
   * The decorating template _X_ need to take its own base class as template
   * parameter. Typically, operations on _X_ will be defined in a recursive fashion,
   * calling down into this templated base class. To support this, an instantiation
   * of _X_ with the empty type sequence is generated for detecting recursion end
   * (built as innermost decorator, i.e. the immediate subclass of TUP)
   */
  template
    < template<class,class,class, uint> class _X_   ///< user provided template<Type, Base, TupleType, arg-idx>
    , typename TYPES                                ///< Sequence of types to use within the Accessor
    , class TUP =Tuple<TYPES>                       ///< the tuple type to build on
    , uint i = 0                                    ///< tuple element index counter
    >
  class BuildTupleAccessor
    {
      // prepare recursion...
      using Head         = typename Split<TYPES>::Head;
      using Tail         = typename Split<TYPES>::Tail;
      using NextBuilder  = BuildTupleAccessor<_X_, Tail,TUP, i+1>;
      using NextAccessor = typename NextBuilder::Product;
    public:
      
      /** type of the product created by this template.
       *  Will be a subclass of TUP */
      using Product = _X_< Head            // the type to use for this accessor
                         , NextAccessor    // the base type to inherit from
                         , TUP             // the tuple type we build upon
                         , i               // current element index
                         >;
    };
  
  
  template
    < template<class,class,class, uint> class _X_
    , class TUP
    , uint i
    >
  class BuildTupleAccessor< _X_, Types<>, TUP, i>
    {
    public:
      using Product = _X_<NullType, TUP, TUP, i>;   // Note: i == tuple size
      
    };
  
  
  
  /**
   * Helper to dump tuple contents.
   * Defined to act as "Accessor" for BuildTupleAccessor, this helper template
   * allows to create a recursive operation to invoke string conversion on
   * all elements within any given tuple.
   */
  template
    < typename TY
    , class BASE
    , class TUP
    , uint idx
    >
  struct TupleElementDisplayer
    : BASE
    {
      using BASE::BASE;
      
      std::string
      dump (std::string const& prefix ="(")  const
        {
          return BASE::dump (prefix + util::toString(std::get<idx>(*this))+",");
        }
    };
  
  template<class TUP, uint n>
  struct TupleElementDisplayer<NullType, TUP, TUP, n>
    : TUP
    {
      TupleElementDisplayer (TUP const& tup)
        : TUP(tup)
        { }
      
      std::string
      dump (std::string const& prefix ="(")  const
        {
          if (1 < prefix.length())
            // remove the trailing comma
            return prefix.substr (0, prefix.length()-1) +")";
          else
            return prefix+")";
        }
    };
  
  
  /**
   * convenience function to dump a given tuple's contents.
   * Using the BuildTupleAccessor, we layer a stack of Instantiations of
   * the TupleElementDisplayer temporarily on top of the given tuple,
   * just to invoke a recursive call chain through this layers
   * and get a string representation of each element in the
   * tuple.
   */
  template<typename...TYPES>
  inline std::string
  dump (std::tuple<TYPES...> const& tuple)
  {
    using BuildAccessor = BuildTupleAccessor<TupleElementDisplayer, Types<TYPES...>>;
    using Displayer     = typename BuildAccessor::Product ;
    
    return static_cast<Displayer const&> (tuple)
          .dump();
  }
  
  
  
}} // namespace lib::meta


// add a specialisation to enable tuple string conversion
namespace util {
  
  template<typename...TYPES>
  struct StringConv<std::tuple<TYPES...>>
    {
      static std::string
      invoke (std::tuple<TYPES...> const& tuple) noexcept
        try {
          return "«"+typeStr(tuple)
               + "»──" + lib::meta::dump (tuple);
        }
        catch(...) { return FAILURE_INDICATOR; }
    };
  
  
} // namespace util
#endif /*LIB_META_TUPLE_HELPER_H*/
