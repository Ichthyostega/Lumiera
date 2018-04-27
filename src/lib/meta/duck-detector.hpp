/*
  DUCK-DETECTOR.hpp  -  helpers for statically detecting properties of a type

  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file duck-detector.hpp
 ** Metaprogramming helpers to check for specific properties of a type in question.
 ** Building upon the "SFINAE" principle, it is possible to create \em metafunction templates,
 ** which answer some questions about a given type at compile time. A lot of generic predicates of
 ** this kind can be found in the `<type_traits>` library (standard since C++11). At times though,
 ** you want to ask more specific questions, like e.g. "does this type provide an operation quack() "?
 ** Because, if we can get a `bool` answer to such a question _at compile time,_ we can use
 ** `std::enable_if` to pick a special implementation based on the test result. Together, these
 ** techniques allow to adopt a duck-typed programming style, where an arbitrary object is allowed
 ** to enter a given API function, provided this object supports some specific operations.
 ** 
 ** While C++ certainly isn't a dynamic language and does not provide any kind of run time introspection,
 ** doing such check-and branch at compile time allows to combine flexibility as known from dynamic
 ** languages with static type safety, which is compelling. We can generate similar implementation
 ** for types not further related by inheritance. Building on this, we're able to emulate some
 ** of the features enabled by type classes (or "concepts").
 ** 
 ** # how the implementation works
 ** 
 ** Most of these trait templates rely on a creative use of function overloading. The C++ standard
 ** requires the compiler _silently to drop_ any candidate of overload resolution which has gotten
 ** an invalid function signature as a result of instantiating a template (type). This rule allows
 ** us to set up kind of a "honey pot" for the compiler: we present two overloaded candidate functions
 ** with a different return type; by investigating the resulting return type we're able to figure
 ** out the overload actually picked by the compiler.
 ** 
 ** This header provides some pre-configured tests, available as macros. Each of them contains
 ** a template based on the described setup, containing a _probe type expression_ at some point.
 ** The key is to build this probe expression in a way that it is valid if and only if the
 ** type in question exhibits a specific property.
 ** 
 ** - if the type should contain a nested type or typedef with a specific name, we simply use
 **   this nested type in the signature of the overloaded function
 ** - if the type should contain a _member_ with a specific name, we initialise a member pointer
 **   within a probe template with this member (if there isn't such a member, the probe template
 **   initialisation fails and the other function overload gets picked)
 ** - as an extension to this approach, we can even declare a member function pointer with a
 **   specific function signature and then try to assign the named member. This allows even
 **   to determine if a member function of a type in question has the desired signature.
 ** 
 ** All these detection building blocks are written such as to provide a bool member `::value`,
 ** which is in accordance to the conventions of C++11 metaprogramming. I.e. you can immediately
 ** use them within `std::enable_if`
 ** 
 ** # some pitfalls to consider
 ** 
 ** @warning The generated metafunctions all yield the `false` value by default.
 **          Effectively this means that an error in the test expression might go unnoticed;
 **          you'd be better off explicitly checking the detection result by an unit test.
 ** 
 ** There are several *typical problems* to care about
 ** - none of these tests can detect any private members
 ** - the name-only detectors will fail if the name is ambiguous
 ** - a member can be both a variable or a function of that name
 ** - function signatures need to match precisely, including const modifiers
 ** - the generated metafunction (template) uses a type parameter 'TY', which could
 **   shadow or conflict with an type parameter in the enclosing scope
 ** - some of the detectors _require a complete type_ to work properly. They create a
 **   pointer-to-member or invoke `sizeof()`. In regular code, doing such on an incomplete
 **   type would provoke a _compilation failure_ -- however, here this code gets evaluated
 **   in a SFINAE context, which means, it will fail silently and thus produce a wrong
 **   detection result. This can be quite *insidious* when relying on the proper detection
 **   to pick the right implementation/specialisation; especially when instantiating
 **   _mutually dependent_ templates, the distinction between "complete" and "incomplete"
 **   can be rather arbitrary while in the process of instantiation.
 ** - the member and function checks rely on member pointers, which generally refer to
 **   the explicit static type. These checks won't see any inherited members / functions.
 ** - obviously, all those checks are never able to detect anything depending on runtime
 **   types or RTTI
 ** 
 ** @see util-foreach.hpp usage example
 ** @see iter-tree-explorer.hpp (example: is_StateCore<SRC>)
 ** @see duck-detector-test.cpp
 ** @see duck-detector-extension-test.cpp
 ** 
 */


#ifndef LIB_META_DUCK_DETECTOR_H
#define LIB_META_DUCK_DETECTOR_H


#include "lib/meta/util.hpp"





/** Detector for a nested type.
 *  Defines a metafunction (template), allowing to detect
 *  if a type TY in question has a nested type or typedef
 *  with the given name. To answer this question, instantiate
 *  resulting HasNested_XXX template with the type in question
 *  and check the static bool value field.
 * @warning none of these checks can not detect private members
 */
#define META_DETECT_NESTED(_TYPE_)                            \
    template<typename TY>                                      \
    class HasNested_##_TYPE_                                    \
      {                                                          \
                                                                  \
        template<class X>                                          \
        static Yes_t check(typename X::_TYPE_ *);                   \
        template<class>                                              \
        static No_t  check(...);                                      \
                                                                       \
      public:                                                           \
        static const bool value = (sizeof(Yes_t)==sizeof(check<TY>(0))); \
      };



/** Detector for a nested member (field or function).
 *  Defines a metafunction (template), allowing to detect
 *  the presence of a member with the given name within
 *  a type in question.
 * @note this check will likely fail if the name is ambiguous.
 * @warning none of these checks can not detect private members
 */
#define META_DETECT_MEMBER(_NAME_)                         \
    template<typename TY>                                   \
    class HasMember_##_NAME_                                 \
      {                                                       \
        template<typename X,                                   \
                 typename SEL = decltype(&X::_NAME_)>           \
        struct Probe                                             \
          { };                                                    \
                                                                   \
        template<class X>                                           \
        static Yes_t check(Probe<X> * );                             \
        template<class>                                               \
        static No_t  check(...);                                       \
                                                                        \
      public:                                                            \
        static const bool value = (sizeof(Yes_t)==sizeof(check<TY>(0)));  \
      };



/** Detector for a specific member function.
 *  Defines a metafunction (template), allowing to detect
 *  the presence of a member function with the specific
 *  signature, as defined by the parameters.
 * @note this check is not sensible to overloads,
 *       due to the explicitly given argument types
 */
#define META_DETECT_FUNCTION(_RET_TYPE_,_FUN_NAME_,_ARGS_) \
    template<typename TY>                                   \
    class HasFunSig_##_FUN_NAME_                             \
      {                                                       \
        template<typename X, _RET_TYPE_ (X::*)_ARGS_>          \
        struct Probe                                            \
          { };                                                   \
                                                                  \
        template<class X>                                          \
        static Yes_t check(Probe<X, &X::_FUN_NAME_> * );            \
        template<class>                                              \
        static No_t  check(...);                                      \
                                                                       \
      public:                                                           \
        static const bool value = (sizeof(Yes_t)==sizeof(check<TY>(0))); \
      };


/** Detector for a member function with the given name.
 *  Defines a metafunction (template), allowing to detect
 *  the presence of a member function with a specific name,
 *  but without imposing any additional constraints on arguments
 *  and return type. Yet a non-function member will not trigger this detector.
 * @note this check will fail if there are overloads or similar ambiguity
 */
#define META_DETECT_FUNCTION_NAME(_FUN_NAME_)              \
    template<typename TY>                                   \
    class HasFunName_##_FUN_NAME_                            \
      {                                                       \
        template<typename SEL>                                 \
        struct Probe;                                           \
        template<class C, typename RET, typename...ARGS>         \
        struct Probe<RET (C::*) (ARGS...)>                        \
          {                                                        \
            using Match = void;                                     \
          };                                                         \
        template<class C, typename RET, typename...ARGS>              \
        struct Probe<RET (C::*) (ARGS...)  const>                      \
          {                                                             \
            using Match = void;                                          \
          };                                                              \
                                                                           \
        template<class X>                                                   \
        static Yes_t check(typename Probe<decltype(&X::_FUN_NAME_)>::Match * ); \
        template<class>                                                       \
        static No_t  check(...);                                               \
                                                                                \
      public:                                                                    \
        static const bool value = (sizeof(Yes_t)==sizeof(check<TY>(0)));          \
      };


/** Detector for an argument-less member function with the given name.
 *  Defines a metafunction (template), allowing to detect a member function
 *  taking no arguments, and with arbitrary return type.
 * @remarks the presence of overloads is irrelevant, since we explicitly
 *          from an invocation to that function (within `decltype`)
 */
#define META_DETECT_FUNCTION_ARGLESS(_FUN_)                \
    template<typename TY>                                   \
    class HasArglessFun_##_FUN_                              \
      {                                                       \
        template<typename X,                                   \
                 typename SEL = decltype(std::declval<X>()._FUN_())>\
        struct Probe                                             \
          { };                                                    \
                                                                   \
        template<class X>                                           \
        static Yes_t check(Probe<X> * );                             \
        template<class>                                               \
        static No_t  check(...);                                       \
                                                                        \
      public:                                                            \
        static const bool value = (sizeof(Yes_t)==sizeof(check<TY>(0)));  \
      };



/** Detector for support of a free-function extension point.
 *  Defines a metafunction (template), allowing to probe if the type
 *  in question supports a specific extension point function. Typically
 *  such functions are injected by some type in a way to be picked up by ADL.
 *  The detection test works by forming an expression to invoke the extension point,
 *  passing the type given as template parameter as function argument. If this expression
 *  type checks, the extension point is assumed to be supported.
 * @warning beware of implicit type conversions
 */
#define META_DETECT_EXTENSION_POINT(_FUN_)                 \
    template<typename TY>                                   \
    class HasExtensionPoint_##_FUN_                          \
      {                                                       \
        template<typename X,                                   \
                 typename SEL = decltype( _FUN_(std::declval<X>()))>\
        struct Probe                                             \
          { };                                                    \
                                                                   \
        template<class X>                                           \
        static Yes_t check(Probe<X> * );                             \
        template<class>                                               \
        static No_t  check(...);                                       \
                                                                        \
      public:                                                            \
        static const bool value = (sizeof(Yes_t)==sizeof(check<TY>(0)));  \
      };



/** Detector for a dereferentiation operator. Works like member detection */
#define META_DETECT_OPERATOR_DEREF()                       \
    template<typename TY>                                   \
    class HasOperator_deref                                  \
      {                                                       \
        template<typename X, int i = sizeof(&X::operator*)>    \
        struct Probe                                            \
          { };                                                   \
                                                                  \
        template<class X>                                          \
        static Yes_t check(Probe<X> * );                            \
        template<class>                                              \
        static No_t  check(...);                                      \
                                                                       \
      public:                                                           \
        static const bool value = (sizeof(Yes_t)==sizeof(check<TY>(0))); \
      };



/** Detector for a prefix increment operator.
 * @note there is a twist: because of the prefix and postfix version of increment,
 *       detection through member-check will fail when both are present (ambiguity).
 *       OTOH, when using function signature detection, the return type must match.
 *       The latter fails in the common situation, when the increment operator was
 *       mixed in through some base class. As a pragmatic solution, we do both kinds
 *       of tests; so either remove one of the operators (typically postfix), or
 *       add an forwarding override in the class to be checked */
#define META_DETECT_OPERATOR_INC()                    \
    template<typename TY>                              \
    class HasOperator_inc                               \
      {                                                  \
        template<typename X, X& (X::*)(void)>             \
        struct Probe_1                                     \
          { };                                              \
        template<typename X, int i = sizeof(&X::operator++)> \
        struct Probe_2                                        \
          { };                                                 \
                                                                \
        template<class X>                                        \
        static Yes_t check1(Probe_1<X, &X::operator++> * );       \
        template<class>                                            \
        static No_t  check1(...);                                   \
        template<class X>                                            \
        static Yes_t check2(Probe_2<X> * );                           \
        template<class>                                                \
        static No_t  check2(...);                                       \
                                                                         \
      public:                                                             \
        static const bool value = (sizeof(Yes_t)==sizeof(check1<TY>(0))    \
                                 ||sizeof(Yes_t)==sizeof(check2<TY>(0)));   \
      };



#endif
