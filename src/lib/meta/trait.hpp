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
 ** It incurs only modest header inclusion overhead
 ** @warning be sure not to jeopardise that!
 ** 
 ** \par unwrapping
 ** Strip away all kinds of type adornments, like const, reference, pointer, smart-ptr.
 ** The accompanying \ref lib::meta::unwrap() function can be used to accept "stuff
 ** packaged in various forms". The \ref Strip template packages this ability in various
 ** degrees for metaprogramming
 ** @warning these helpers can be quite dangerous, as they silently break
 **          any protective barriers (including lifecycle managing smart-ptrs)
 ** 
 ** \par string conversion
 ** a set of trait templates to categorise arbitrary types with respect to
 ** the ability for string conversions
 ** 
 ** \par ability to iterate
 ** these traits [can be used](\ref util-foreach.hpp) to build the notion of a
 ** generic container -- basically anything that can be enumerated.
 ** Within Lumiera, we frequently use our own concept of "iterability",
 ** known as ["Lumiera Forward Iterator"](\ref iter-adapter.hpp). These
 ** helpers here allow to unify this concept with the "Range"
 ** concept from the standard library (`begin()` and `end()`)
 ** 
 ** @see MetaUtils_test
 ** @see format-obj.hpp string representation for _anything_
 ** @see meta/util.hpp very basic metaprogramming helpers
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
  template<class X, class D> class unique_ptr;
}
namespace lib{
  template<class X, class B>  class P;
  
  namespace hash {
    class LuidH;
  }
  namespace time {
    class TimeValue;
    class Duration;
}}
namespace steam {
namespace mobject{
  template<class X, class B>  class Placement;
}}


namespace lib {
namespace meta {
  
  using std::remove_cv;
  using std::remove_cv_t;
  using std::remove_pointer;
  using std::remove_pointer_t;
  using std::remove_reference;
  using std::remove_reference_t;
  using std::is_pointer;
  using std::is_base_of;
  using std::is_convertible;
  using std::is_constructible;
  using std::is_floating_point;
  using std::is_arithmetic;
  using std::is_unsigned;
  using std::is_signed;
  using std::is_class;
  using std::is_same;
  using std::__not_;
  using std::__and_;
  using std::__or_;
  
  
  /**
   * Helper for type analysis and convenience accessors:
   * attempts to extract a base type from various wrappers.
   * Additionally allows to extract/deref the wrapped element.
   * @note can also be used as a meta function to detect "anything wrapped"
   * @warning strips away any const
   * @warning also strips away smart-ptrs and lifecycle managers!
   */
  template<typename X>
  struct Unwrap
    : std::false_type
    {
      using Type = X;
      
      static  X&
      extract (X const& x)
        {
          return const_cast<X&> (x);
        }
    };
  
  template<>
  struct Unwrap<void>   ///< @note we can't unwrap void!
    : std::false_type
    {
      using Type = void;
    };
  
  template<typename X>
  struct Unwrap<X*>
    : std::true_type
    {
      using Type = typename std::remove_cv<X>::type;
      
      static Type&
      extract (const X* ptr)
        {
          ASSERT (ptr);
          return const_cast<Type&> (*ptr);
        }
    };
  
  template<typename X>
  struct Unwrap<boost::reference_wrapper<X>>
    : std::true_type
    {
      using Type = X;
      
      static X&
      extract (boost::reference_wrapper<X> wrapped)
        {
          return wrapped;
        }
    };
  
  template<typename X>
  struct Unwrap<std::reference_wrapper<X>>
    : std::true_type
    {
      using Type = X;
      
      static X&
      extract (std::reference_wrapper<X> wrapped)
        {
          return wrapped;
        }
    };
  
  template<typename X, class D>
  struct Unwrap<std::unique_ptr<X,D>>
    : std::true_type
    {
      using Type = X;
      
      static X&
      extract (std::unique_ptr<X,D> const& ptr)
        {
          ASSERT (ptr);
          return *ptr;
        }
    };
  
  template<typename X>
  struct Unwrap<std::shared_ptr<X>>
    : std::true_type
    {
      using Type = X;
      
      static X&
      extract (std::shared_ptr<X> const& ptr)
        {
          ASSERT (ptr);
          return *ptr;
        }
    };
  
  template<typename X, class B>
  struct Unwrap<P<X, B>>
    : std::true_type
    {
      using Type = X;
      
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
      using TypeUnconst  = typename std::remove_cv<X>                 ::type;
      using TypeReferred = typename std::remove_reference<TypeUnconst>::type;
      using TypePointee  = typename std::remove_pointer<TypeReferred> ::type;
      using TypePlain    = typename std::remove_cv<TypePointee>       ::type;
      
      using Type         = typename Unwrap<TypePlain>::Type;
    };
  
  
  
  
  /** Type definition helper for pointer and reference types.
   *  Allows to create a member field and to get the basic type
   *  irrespective if the given type is plain, pointer or reference
   * @note we _do treat pointers specific_ though; a pointer is itself
   *       a value and the pointer-indirection is _not_ stripped.
   *       (use meta::Strip to radically strip all adornments)
   */
  template<typename TY>
  struct RefTraits
    {
      typedef TY  Value;
      typedef TY* Pointer;
      typedef TY& Reference;
    };
  
  template<typename TY>
  struct RefTraits<TY *>
    {
      typedef TY*  Value;
      typedef TY** Pointer;
      typedef TY*& Reference;
    };
  
  template<typename TY>
  struct RefTraits<TY &>
    {
      typedef TY  Value;
      typedef TY* Pointer;
      typedef TY& Reference;
    };
  
  template<typename TY>
  struct RefTraits<TY &&>
    {
      typedef TY  Value;
      typedef TY* Pointer;
      typedef TY& Reference;
    };
  
  
  
  
  
  
  
  /* ==== Traits ==== */
  
  /** compare unadorned types, disregarding const and references */
  template<typename T, typename U>
  struct is_basically
    : is_same <typename Strip<T>::TypeReferred
              ,typename Strip<U>::TypeReferred>
    { };
  
  /** verify compliance to an interface by subtype check */
  template<typename S, typename I>
  struct is_Subclass
    : __or_< __and_< is_class<I>
                   , is_class<S>
                   , is_base_of<I,S>
                   >
           , is_same<I,S>
           >
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
    : __and_<is_class<typename Strip<X>::TypePlain>
            ,__not_<is_pointer<X>>
            ,__not_<can_lexical2string<X>>
            >
    { };
  
  
  
  /** detect smart pointers */
  template<typename X>
  struct is_smart_ptr
    : std::false_type
    { };
  
  template<typename T>
  struct is_smart_ptr<std::shared_ptr<T>>
    : std::true_type
    { };
  
  template <typename T, typename D>
  struct is_smart_ptr<std::unique_ptr<T,D>>
    : std::true_type
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
  
  template<typename TAR>
  struct is_narrowingInit<lib::hash::LuidH, TAR>
    : __or_<is_arithmetic<TAR>
           ,is_floating_point<TAR>
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
  TRAIT_IS_NARROWING (int64_t, char)
  TRAIT_IS_NARROWING (int32_t, int16_t)
  TRAIT_IS_NARROWING (int32_t, int8_t)
  TRAIT_IS_NARROWING (int32_t, char)
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
  
  TRAIT_IS_NARROWING (double, lib::time::TimeValue)
  TRAIT_IS_NARROWING (double, lib::time::Duration)
  
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
      using  Type = typename Strip<T>::Type;
       
      META_DETECT_NESTED(value_type);
      META_DETECT_OPERATOR_DEREF();
      META_DETECT_OPERATOR_INC();
      
    public:
      enum{ value = std::is_constructible<bool, Type>::value
                and HasNested_value_type<Type>::value
                and HasOperator_deref<Type>::value
                and HasOperator_inc<Type>::value
          };
    };
  
  
  
  /** Trait template to detect a type usable with the STL for-each loop.
   *  Basically we're looking for the functions to get the begin/end iterator
   */
  template<typename T>
  class can_STL_ForEach
    {
      using Type = typename Strip<T>::Type;
      
      struct is_iterable
        {
          META_DETECT_NESTED(iterator);
          META_DETECT_FUNCTION(typename X::iterator, begin,(void));
          META_DETECT_FUNCTION(typename X::iterator, end  ,(void));
          
          enum { value = HasNested_iterator<Type>::value
                     and HasFunSig_begin<Type>::value
                     and HasFunSig_end<Type>::value
           };
        };
      
      struct is_noexcept_iterable
        {
          META_DETECT_NESTED(iterator);
          META_DETECT_FUNCTION(typename X::iterator, begin,(void) noexcept);
          META_DETECT_FUNCTION(typename X::iterator, end  ,(void) noexcept);
          
          enum { value = HasNested_iterator<Type>::value
                     and HasFunSig_begin<Type>::value
                     and HasFunSig_end<Type>::value
           };
        };
      
      struct is_const_iterable
        {
          META_DETECT_NESTED(const_iterator);
          META_DETECT_FUNCTION(typename X::const_iterator, begin,(void) const);
          META_DETECT_FUNCTION(typename X::const_iterator, end  ,(void) const);
          
          enum { value = HasNested_const_iterator<Type>::value
                     and HasFunSig_begin<Type>::value
                     and HasFunSig_end<Type>::value
           };
        };
      
      struct is_const_noexcept_iterable
        {
          META_DETECT_NESTED(const_iterator);
          META_DETECT_FUNCTION(typename X::const_iterator, begin,(void) const noexcept);
          META_DETECT_FUNCTION(typename X::const_iterator, end  ,(void) const noexcept);
          
          enum { value = HasNested_const_iterator<Type>::value
                     and HasFunSig_begin<Type>::value
                     and HasFunSig_end<Type>::value
           };
        };
      
      
    public:
      enum { value = is_iterable::value
                  or is_const_iterable::value
                  or is_noexcept_iterable::value
                  or is_const_noexcept_iterable::value
           };
    };
  
  
  /** Trait template to detect a type also supporting STL-style backwards iteration */
  template<typename T>
  class can_STL_backIteration
    {
      using Type = typename Strip<T>::Type;
      
      struct is_backIterable
        {
          META_DETECT_NESTED(reverse_iterator);
          META_DETECT_FUNCTION(typename X::reverse_iterator, rbegin,(void));
          META_DETECT_FUNCTION(typename X::reverse_iterator, rend  ,(void));
          
          enum { value = HasNested_reverse_iterator<Type>::value
                     and HasFunSig_rbegin<Type>::value
                     and HasFunSig_rend<Type>::value
           };
        };
      
      struct is_noexcept_backIterable
        {
          META_DETECT_NESTED(reverse_iterator);
          META_DETECT_FUNCTION(typename X::reverse_iterator, rbegin,(void) noexcept);
          META_DETECT_FUNCTION(typename X::reverse_iterator, rend  ,(void) noexcept);
          
          enum { value = HasNested_reverse_iterator<Type>::value
                     and HasFunSig_rbegin<Type>::value
                     and HasFunSig_rend<Type>::value
           };
        };
      
      struct is_const_backIterable
        {
          META_DETECT_NESTED(const_reverse_iterator);
          META_DETECT_FUNCTION(typename X::const_reverse_iterator, rbegin,(void) const);
          META_DETECT_FUNCTION(typename X::const_reverse_iterator, rend  ,(void) const);
          
          enum { value = HasNested_const_reverse_iterator<Type>::value
                     and HasFunSig_rbegin<Type>::value
                     and HasFunSig_rend<Type>::value
           };
        };
      
      struct is_const_noexcept_backIterable
        {
          META_DETECT_NESTED(const_reverse_iterator);
          META_DETECT_FUNCTION(typename X::const_reverse_iterator, rbegin,(void) const noexcept);
          META_DETECT_FUNCTION(typename X::const_reverse_iterator, rend  ,(void) const noexcept);
          
          enum { value = HasNested_const_reverse_iterator<Type>::value
                     and HasFunSig_rbegin<Type>::value
                     and HasFunSig_rend<Type>::value
           };
        };
      
      
    public:
      enum { value = is_backIterable::value
                  or is_const_backIterable::value
                  or is_noexcept_backIterable::value
                  or is_const_noexcept_backIterable::value
           };
    };
  
  
  
}} // namespace lib::meta
#endif
