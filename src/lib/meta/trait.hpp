/*
  TRAIT.hpp  -  type handling and type detection helpers

  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file trait.hpp
 ** Helpers for type detection, type rewriting and metaprogramming.
 ** This header is a collection of frequently used templates for working with types.
 ** It incurs only modest header inclusion overhead (be sure not to jeopardise that!).
 ** 
 ** \par unwrapping
 ** Strip away all kinds of type adornments, like const, reference, pointer, smart-ptr.
 ** The accompanying \ref unwrap() function can be used to accept "stuff packaged
 ** in various forms". The \ref Strip template packages this ability in various
 ** degrees for metaprogramming
 ** @warning these helpers can be quite dangerous, as they silently break
 **          any protective barriers (including lifecycle managing smart-ptrs)
 ** 
 ** \par string conversion
 ** a set of trait templates to categorise arbitrary types with respect to
 ** the ability for string conversions
 ** 
 ** \par ability to iterate
 ** these traits [can be used](util-foreach.hpp) to build the notion of a
 ** generic container -- basically anything that can be enumerated.
 ** Within Lumiera, we frequently use our own concept of "iterability",
 ** known as ["Lumiera Forward Iterator"](iter-adapter.hpp). These
 ** helpers here allow to unify this concept with the "Range"
 ** concept from the standard library (`begin()` and `end()`)
 ** 
 ** @see MetaUtils_test
 ** @see \rem format-obj.hpp string representation for _anything_
 ** @see \ref lib/meta/utils.hpp very basic metaprogramming helpers
 ** @see typelist.hpp
 ** 
 */


#ifndef LIB_META_TRAIT_H
#define LIB_META_TRAIT_H


#include "lib/meta/util.hpp"
#include "lib/meta/duck-detector.hpp"

#include <type_traits>


//Forward declarations for the Unwrap helper....
namespace boost{
  template<class X> class reference_wrapper; 
}
namespace std {
  template<class X> class reference_wrapper;
  template<class X> class shared_ptr; 
}
namespace lib{
  template<class X, class B>  class P;
  
  namespace hash {
    class LuidH;
}}
namespace proc {
namespace mobject{
  template<class X, class B>  class Placement;
}}


namespace lib {
namespace meta {
  
  using std::remove_cv;
  using std::remove_pointer;
  using std::remove_reference;
  using std::is_convertible;
  using std::is_constructible;
  using std::is_floating_point;
  using std::is_arithmetic;
  using std::is_unsigned;
  using std::is_signed;
  using std::is_same;
  using std::__not_;
  using std::__and_;
  using std::__or_;
  
  
  /** 
   * Helper for type analysis and convenience accessors:
   * attempts to extract a base type from various wrappers.
   * Additionally allows to extract/deref the wrapped element.
   * @warning strips away any const
   * @warning also strips away smart-ptrs and lifecycle managers!
   */
  template<typename X>
  struct Unwrap
    {
      typedef X Type;
      
      static  X&
      extract (X const& x)
        {
          return const_cast<X&> (x);
        }
    };
  
  template<>
  struct Unwrap<void>   ///< @note we can't unwrap void!
    {
      typedef void Type;
    };
  
  template<typename X>
  struct Unwrap<X*>
    {
      typedef typename std::remove_cv<X>::type Type;
      
      static Type&
      extract (const X* ptr)
        {
          ASSERT (ptr);
          return const_cast<Type&> (*ptr);
        }
    };
  
  template<typename X>
  struct Unwrap<boost::reference_wrapper<X> >
    {
      typedef X  Type;
      
      static X&
      extract (boost::reference_wrapper<X> wrapped)
        {
          return wrapped;
        }
    };
  
  template<typename X>
  struct Unwrap<std::reference_wrapper<X> >
    {
      typedef X  Type;
      
      static X&
      extract (std::reference_wrapper<X> wrapped)
        {
          return wrapped;
        }
    };
  
  template<typename X>
  struct Unwrap<std::shared_ptr<X> >
    {
      typedef X  Type;
      
      static X&
      extract (std::shared_ptr<X> ptr)
        {
          ASSERT (ptr);
          return *ptr;
        }
    };
  
  template<typename X, class B>
  struct Unwrap<P<X, B> >
    {
      typedef X  Type;
      
      static X&
      extract (P<X,B> ptr)
        {
          ASSERT (ptr);
          return *ptr;
        }
    };
  
  
  /** convenience shortcut: unwrapping free function.
   *  @return reference to the bare element.
   *  @warning this function is dangerous: it strips away
   *           any managing smart-ptr and any const!
   *           You might even access and return a
   *           reference to an anonymous temporary.
   */
  template<typename X>
  typename Unwrap<X>::Type&
  unwrap (X const& wrapped)
  {
    return Unwrap<X>::extract(wrapped);
  }
  
  
  
  
  /** Helper for type analysis: tries to strip all kinds of type adornments */
  template<typename X>
  struct Strip
    {
      typedef typename std::remove_cv<X>                 ::type TypeUnconst;
      typedef typename std::remove_reference<TypeUnconst>::type TypeReferred;
      typedef typename std::remove_pointer<TypeReferred> ::type TypePointee;
      typedef typename std::remove_cv<TypePointee>       ::type TypePlain;
      
      typedef typename Unwrap<TypePlain>                   ::Type Type;
    };
  
  
  
  
  /** Type definition helper for pointer and reference types.
   *  Allows to create a member field and to get the basic type
   *  irrespective if the given type is plain, pointer or reference
   */
  template<typename TY>
  struct RefTraits
    {
      typedef TY* pointer;
      typedef TY& reference;
      typedef TY  value_type;
    };
  
  template<typename TY>
  struct RefTraits<TY *>
    {
      typedef TY* pointer;
      typedef TY& reference;
      typedef TY  value_type;
    };
  
  template<typename TY>
  struct RefTraits<TY &>
    {
      typedef TY* pointer;
      typedef TY& reference;
      typedef TY  value_type;
    };
  
  
  
  
  
  
  
  /* ==== Traits ==== */
  
  /** compare unadorned types, disregarding const and references */
  template<typename T, typename U>
  struct is_basically
    : is_same <typename Strip<T>::TypePlain
              ,typename Strip<U>::TypePlain>
    { };
  
  /** detect various flavours of string / text data */
  template<typename X>
  struct is_StringLike
    : __or_< is_basically<X, std::string>
           , is_convertible<X, const char*>
           >
    { };
  
  /** types able to be lexically converted to string representation
   * @note this compile-time trait can't predict if such an conversion
   *    to string will be successful at runtime; indeed it may throw,
   *    so you should additionally guard the invocation with try-catch!
   * @remarks this template is relevant for guarding `lexical_cast<..>` expressions.
   *    Such an expression won't even compile for some types, because of missing or
   *    ambiguous output operator(s). Ideally, there would be some automatic detection
   *    (relying on the existence of an `operator<<` for the given type. But at my
   *    first attempt in 2009 (commit 1533e5bd0) I couldn't make this work, so I
   *    fell back on just declaring all classes of types which are known to work
   *    with lexical_cast to string.
   * @remarks Meanwhile (2016) I think this is an adequate and robust solution
   *    and here to stay. Based on this, I'll add a generic overload for the
   *    output stream inserter `operator<<` to use custom string conversions;
   *    this trait is essential to exclude types which can be printed as-is.
   */
  template<typename X>
  struct can_lexical2string
    : __or_< is_arithmetic<X>
           , is_StringLike<X>
           >
    { };
  
  template<typename X>
  struct use_LexicalConversion
    : __and_<can_lexical2string<X>
            ,__not_<can_convertToString<X>>
            >
    { };
  
  /** when to use custom string conversions for output streams */
  template<typename X>
  struct use_StringConversion4Stream
    : __and_<std::is_class<typename Strip<X>::TypePlain>
            ,__not_<std::is_pointer<X>>
            ,__not_<can_lexical2string<X>>
            >
    { };
  
  
  
  
  
  
  template<typename NUM>
  struct is_nonFloat
    : __and_<is_arithmetic<NUM>
            ,__not_<is_floating_point<NUM>>
            >
    { };
  
  /** temporary workaround for GCC [Bug-63723], necessary until CGG-5
   * @remarks The problem is that GCC emits a warning on narrowing conversion,
   *  instead of letting the SFINAE substitution fail. This can be considered
   *  questionable behaviour, since the usual implementation of a `is_convertible`
   *  trait uses initialisation from a brace enclosed list, where C++11 prohibits
   *  narrowing conversions. Now the problem is, that we'll use such traits checks
   *  to remove such  _impossble_ cases from generated trampoline tables or visitor
   *  double dispatch implementations. Thus, for one we get lots of warnings at that
   *  point when generating those trampoline tables (at initialisation), while it
   *  is not clear we'll trigger those cases, and, when we do, we'll get narrowing
   *  conversions in a context where we're unable to cope with them or protect
   *  ourselves against spurious conversions.
   *  What follows is a quick-n-dirty hack to remove such unwanted conversions. 
   * 
   * [Bug-63723]: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=63723
   */
  template<typename SRC, typename TAR>
  struct is_narrowingInit
    : __or_<__and_<is_unsigned<SRC>, is_signed<TAR>>
           ,__and_<is_signed<SRC>, is_unsigned<TAR>>
           ,__and_<is_nonFloat<SRC>, is_floating_point<TAR>>
           ,__and_<is_floating_point<SRC>, is_nonFloat<TAR>>
           ,__not_<is_constructible<TAR, SRC>>
           >
    { };
  
#define TRAIT_IS_NARROWING(_SRC_, _TAR_) \
  template<>                              \
  struct is_narrowingInit<_SRC_, _TAR_>    \
    : std::true_type                        \
    { };
  
  TRAIT_IS_NARROWING (int64_t, int32_t)
  TRAIT_IS_NARROWING (int64_t, int16_t)
  TRAIT_IS_NARROWING (int64_t, int8_t)
  TRAIT_IS_NARROWING (int32_t, int16_t)
  TRAIT_IS_NARROWING (int32_t, int8_t)
  TRAIT_IS_NARROWING (int16_t, int8_t)
  TRAIT_IS_NARROWING (int16_t, short)
  TRAIT_IS_NARROWING (int16_t, char)
  
  TRAIT_IS_NARROWING (uint64_t, uint32_t)
  TRAIT_IS_NARROWING (uint64_t, uint16_t)
  TRAIT_IS_NARROWING (uint64_t, uint8_t)
  TRAIT_IS_NARROWING (uint32_t, uint16_t)
  TRAIT_IS_NARROWING (uint32_t, uint8_t)
  TRAIT_IS_NARROWING (uint16_t, uint8_t)
  TRAIT_IS_NARROWING (uint16_t, ushort)
  
  TRAIT_IS_NARROWING (double, float)
  
  TRAIT_IS_NARROWING (lib::hash::LuidH, int64_t)
  TRAIT_IS_NARROWING (lib::hash::LuidH, int32_t)
  TRAIT_IS_NARROWING (lib::hash::LuidH, int16_t)
  TRAIT_IS_NARROWING (lib::hash::LuidH, int8_t)
  TRAIT_IS_NARROWING (lib::hash::LuidH, char)
  TRAIT_IS_NARROWING (lib::hash::LuidH, uint16_t)
  TRAIT_IS_NARROWING (lib::hash::LuidH, uint8_t)
  TRAIT_IS_NARROWING (lib::hash::LuidH, double)
  TRAIT_IS_NARROWING (lib::hash::LuidH, float)
  
#undef TRAIT_IS_NARROWING
  
  
  
  
  
  
  /* ====== generic iteration support ====== */
  
  /** Trait template to detect a type usable immediately as
   *  "Lumiera Forward Iterator" in a specialised for-each loop
   *  This is just a heuristic, based on some common properties
   *  of such iterators; it is enough to distinguish it from an 
   *  STL container, but can certainly be refined.
   */
  template<typename T>
  class can_IterForEach
    {
      typedef typename Strip<T>::Type Type;
       
      META_DETECT_NESTED(value_type);
      META_DETECT_OPERATOR_DEREF();
      META_DETECT_OPERATOR_INC();
      
    public:
      enum{ value = std::is_convertible<Type, bool>::value
                 && HasNested_value_type<Type>::value
                 && HasOperator_deref<Type>::value
                 && HasOperator_inc<Type>::value
          };
    };
  
  
  
  /** Trait template to detect a type usable with the STL for-each loop.
   *  Basically we're looking for the functions to get the begin/end iterator
   */
  template<typename T>
  class can_STL_ForEach
    {
      typedef typename Strip<T>::Type Type;
       
      struct is_iterable
        {
          META_DETECT_NESTED(iterator);
          META_DETECT_FUNCTION(typename X::iterator, begin,(void));
          META_DETECT_FUNCTION(typename X::iterator, end  ,(void));
          
          enum { value = HasNested_iterator<Type>::value
                      && HasFunSig_begin<Type>::value
                      && HasFunSig_end<Type>::value
           };
        };
      
      struct is_const_iterable
        {
          META_DETECT_NESTED(const_iterator);
          META_DETECT_FUNCTION(typename X::const_iterator, begin,(void) const);
          META_DETECT_FUNCTION(typename X::const_iterator, end  ,(void) const);
          
          enum { value = HasNested_const_iterator<Type>::value
                      && HasFunSig_begin<Type>::value
                      && HasFunSig_end<Type>::value
           };
        };
      
      
    public:
      enum { value = is_iterable::value
                  || is_const_iterable::value
           };
    };
  
  
  /** Trait template to detect a type also supporting STL-style backwards iteration */
  template<typename T>
  class can_STL_backIteration
    {
      typedef typename Strip<T>::Type Type;
      
      struct is_backIterable
        {
          META_DETECT_NESTED(reverse_iterator);
          META_DETECT_FUNCTION(typename X::reverse_iterator, rbegin,(void));
          META_DETECT_FUNCTION(typename X::reverse_iterator, rend  ,(void));
          
          enum { value = HasNested_reverse_iterator<Type>::value
                      && HasFunSig_rbegin<Type>::value
                      && HasFunSig_rend<Type>::value
           };
        };
      
      struct is_const_backIterable
        {
          META_DETECT_NESTED(const_reverse_iterator);
          META_DETECT_FUNCTION(typename X::const_reverse_iterator, rbegin,(void) const);
          META_DETECT_FUNCTION(typename X::const_reverse_iterator, rend  ,(void) const);
          
          enum { value = HasNested_const_reverse_iterator<Type>::value
                      && HasFunSig_rbegin<Type>::value
                      && HasFunSig_rend<Type>::value
           };
        };
      
      
    public:
      enum { value = is_backIterable::value
                  || is_const_backIterable::value
           };
    };
  
  
  
}} // namespace lib::meta
#endif
