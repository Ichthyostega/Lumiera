/*
  DUCK-DETECTOR.hpp  -  helpers for statically detecting properties of a type
 
  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>
 
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


/** @file duck-detector.hpp
 ** Metaprogramming helpers to check for specific properties of a type in question.
 ** Building upon the "SFINE" principle, it is possible to create \em metafunction templates,
 ** which answer some questions about a given type at compile time. A lot of generic flavours of
 ** this kind can be found in the boost-type-trait library (part of std::tr1). At times though,
 ** you want to ask more specific questions, like e.g. "does this type provide an operation \c quack() "?
 ** Because, if we can get a \c bool answer to such a question <i>at compile time,</i> we can use
 ** \c boost::enable_if to pick a special implementation based on the test result. Together, these
 ** techniques allow to adopt a duck-typed programming style, where an arbitrary object is allowed
 ** to enter a given API function, provided this object supports some special operations.
 ** 
 ** While C++ certainly isn't a dynamic language and doesn't provide any kind of run time introspection,
 ** doing such check-and branch at compile time allows even to combine such a flexible approach with
 ** static type safety, which is compelling. (The downside is the danger of code bloat, as is with all
 ** template based techniques).
 ** 
 ** \par how the implementation works
 ** 
 ** Most of these trait templates rely on a creative use of function overloading. The C++ standard
 ** requires the compiler <i>silently to drop</i> any candidate of overload resolution which has
 ** gotten an invalid function signature as a result of instantiating a template (type). This allow
 ** us to set up kind of a "trap" for the compiler: we present two overloaded candidate functions
 ** with a different return type; by investigating the resulting return type we're able to figure
 ** out the overload actually picked by the compiler.
 ** 
 ** This header provides some pre-configured tests, available as macros. Each of them contains
 ** a template based on the described setup, containing a \em probe type expression at some point.
 ** The key is to build this probe expression in a way that it's valid only if the type in question
 ** has a specific property
 ** 
 ** - if the type should contain a nested type of typedef with a specific name, we simply use
 **   this nested type in the signature of the overloaded function
 ** - if the type should contain a \em member with a specific name, we initialise a member pointer
 **   within a probe template with this member (if there isn't such a member, the probe template
 **   initialisation fails and the other function overload gets picked)
 ** - as an extension to this approach, we can even declare a member function pointer with a
 **   specific function signature and then try to assign the named member. This allows even
 **   to determine if a member function of a type in question has the desired signature.
 ** 
 ** All these detection building blocks are written such as to provide a bool member \v ::value,
 ** which is in accordance to the conventions of boost metaprogramming. I.e. you can immediately
 ** use them within boost::enable_if
 ** 
 ** @see util-foreach.hpp usage example
 ** @see duck-detector-test.cpp
 ** 
 */


#ifndef LIB_META_DUCK_DETECTOR_H
#define LIB_META_DUCK_DETECTOR_H



namespace lib {
  
  
  /* types for figuring out the overload resolution chosen by the compiler */
  
  typedef char Yes_t;
  struct No_t { char padding[8]; };
  
  ////////////////////////////////////TODO: after fixing the namespace of typelist/meta programming facilities, these can be dropped.
  
  
  
  namespace meta {
    
  } // namespace meta
  
} // namespace lib






/** Detector for a nested type.
 *  Defines a metafunction (template), allowing to detect
 *  if a type TY in question has a nested type or typedef
 *  with the given name. To answer this question, instantiate
 *  resulting HasNested_XXX template with the type in question
 *  and check the static bool value field.
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
 */
#define META_DETECT_MEMBER(_NAME_)                         \
    template<typename TY>                                   \
    class HasMember_##_NAME_                                 \
      {                                                       \
        template<typename X, int i = sizeof(&X::_NAME_)>       \
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



/** Detector for a specific member function.
 *  Defines a metafunction (template), allowing to detect
 *  the presence of a member function with the specific
 *  signature, as defined by the parameters. Note this
 *  check will probably fail if there are overloads
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



/** Detector for a prefix increment operator. Works like function detection */
#define META_DETECT_OPERATOR_INC()                         \
    template<typename TY>                                   \
    class HasOperator_inc                                    \
      {                                                       \
        template<typename X, X& (X::*)(void)>                  \
        struct Probe                                            \
          { };                                                   \
                                                                  \
        template<class X>                                          \
        static Yes_t check(Probe<X, &X::operator++> * );            \
        template<class>                                              \
        static No_t  check(...);                                      \
                                                                       \
      public:                                                           \
        static const bool value = (sizeof(Yes_t)==sizeof(check<TY>(0))); \
      };



#endif
