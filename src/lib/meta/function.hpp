/*
  FUNCTION.hpp  -  metaprogramming utilities for transforming function types

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


/** @file function.hpp
 ** Metaprogramming tools for transforming functor types.
 ** Sometimes it is necessary to build and remould a function signature, e.g. for
 ** creating a functor or a closure based on an existing function of function pointer.
 ** This is a core task of functional programming, but sadly C++ in its current shape
 ** is still lacking in this area. (C++11 significantly improved this situation).
 ** As an \em pragmatic fix, we define here a collection of templates, specialising
 ** them in a very repetitive way for up to 9 function arguments. Doing so enables
 ** us to capture a function, access the return type and argument types as a typelist,
 ** eventually to manipulate them and re-build a different signature, or to create
 ** specifically tailored bindings.
 ** 
 ** If the following code makes you feel like vomiting, please look away,
 ** and rest assured: you aren't alone.
 ** 
 ** @todo get rid of the repetitive specialisations
 **       and use variadic templates to represent the arguments             /////////////////////////////////TICKET #994
 ** 
 ** 
 ** @see control::CommandDef usage example
 ** @see function-closure.hpp generic function application
 ** @see typelist.hpp
 ** @see tuple.hpp
 ** 
 */


#ifndef LIB_META_FUNCTION_H
#define LIB_META_FUNCTION_H

#include "lib/meta/typelist.hpp"

#include <functional>



namespace lib {
namespace meta{

  using std::function;
  
  
  
  
  /**
   * Extract the type information contained in a
   * function or functor type, so it can be manipulated
   * by metaprogramming. Up to 9 function arguments are
   * supported and can be extracted as a type sequence.
   * The bare function signature serving as input can be
   * obtained by capturing a function reference or pointer.
   */
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
  
  
  template< typename RET
          , typename A1
          , typename A2
          , typename A3
          , typename A4
          , typename A5
          , typename A6
          >
  struct FunctionSignature< function<RET(A1,A2,A3,A4,A5,A6)> >
  {
    typedef RET Ret;
    typedef Types<A1,A2,A3,A4,A5,A6> Args;
  };
  
  
  template< typename RET
          , typename A1
          , typename A2
          , typename A3
          , typename A4
          , typename A5
          , typename A6
          , typename A7
          >
  struct FunctionSignature< function<RET(A1,A2,A3,A4,A5,A6,A7)> >
  {
    typedef RET Ret;
    typedef Types<A1,A2,A3,A4,A5,A6,A7> Args;
  };
  
  
  template< typename RET
          , typename A1
          , typename A2
          , typename A3
          , typename A4
          , typename A5
          , typename A6
          , typename A7
          , typename A8
          >
  struct FunctionSignature< function<RET(A1,A2,A3,A4,A5,A6,A7,A8)> >
  {
    typedef RET Ret;
    typedef Types<A1,A2,A3,A4,A5,A6,A7,A8> Args;
  };
  
  
  template< typename RET
          , typename A1
          , typename A2
          , typename A3
          , typename A4
          , typename A5
          , typename A6
          , typename A7
          , typename A8
          , typename A9
          >
  struct FunctionSignature< function<RET(A1,A2,A3,A4,A5,A6,A7,A8,A9)> >
  {
    typedef RET Ret;
    typedef Types<A1,A2,A3,A4,A5,A6,A7,A8,A9> Args;
  };
  
  
  
  
  
  
  /**
   * Helper for uniform access to function signature types.
   * Extract the type information contained in a function or functor type,
   * so it can be manipulated by metaprogramming. The embedded typedefs
   * allow to pick up the return type, the sequence of argument types
   * and the bare function signature type. This template works on
   * anything _function like_, irrespective if the parameter is given
   * as function reference, function pointer, member function pointer,
   * functor object, `std::function` or lambda.
   * 
   * The base case assumes a functor object, i.e. anything with an `operator()`.
   * The following explicit specialisations handle the other cases, which are
   * not objects, but primitive types (function (member) pointers and references).
   * @remarks The key trick of this solution is to rely on `decltype` of `operator()`
   *          and was proposed 10/2011 by user "[kennytm]" in this [stackoverflow].
   * 
   * [kennytm]: http://stackoverflow.com/users/224671/kennytm
   * [stackoverflow] : http://stackoverflow.com/questions/7943525/is-it-possible-to-figure-out-the-parameter-type-and-return-type-of-a-lambda/7943765#7943765 "answer on stackoverflow"
   */
  template<typename FUN>
  struct _Fun
    : _Fun<decltype(&FUN::operator())>
    { };
  
  /** Specialisation for a bare function signature */
  template<typename RET, typename...ARGS>
  struct _Fun<RET(ARGS...)>
    {
      using Ret  = RET;
      using Args = Types<ARGS...>;
      using Sig  = RET(ARGS...);
    };
  /** Specialisation for using a function pointer */
  template<typename SIG>
  struct _Fun<SIG*>
    : _Fun<SIG>
    { };
  
  /** Specialisation when using a function reference */
  template<typename SIG>
  struct _Fun<SIG&>
    : _Fun<SIG>
    { };
  
  /** Specialisation for passing a rvalue reference */
  template<typename SIG>
  struct _Fun<SIG&&>
    : _Fun<SIG>
    { };
  
  /** Specialisation to deal with member pointer to function */
  template<class C, typename RET, typename...ARGS>
  struct _Fun<RET (C::*) (ARGS...)>
    : _Fun<RET(ARGS...)>
    { };
  
  /** Specialisation to handle member pointer to const function;
   *  indirectly this specialisation also handles lambdas,
   *  as redirected by the main template (via `decltype`) */
  template<class C, typename RET, typename...ARGS>
  struct _Fun<RET (C::*) (ARGS...)  const>
    : _Fun<RET(ARGS...)>
    { };
  
  
  
  template<typename FUN>
  struct is_Functor                 { static const bool value = false; };
  template<typename SIG>
  struct is_Functor<function<SIG> > { static const bool value = true;  };
  
  
  
  
  
  
  
  
  /**
   * Build function types from given Argument types.
   * As embedded typedefs, you'll find a tr1 functor #Func
   * and the bare function signature #Sig
   * @param RET the function return type
   * @param ARGS a type sequence describing the arguments
   */
  template<typename RET, typename ARGS>
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
  struct FunctionTypedef<RET, Types<A1>>
  {
    typedef function<RET(A1)> Func;
    typedef          RET Sig(A1);
  };
  
  
  template< typename RET
          , typename A1
          , typename A2
          >
  struct FunctionTypedef<RET, Types<A1,A2>>
  {
    typedef function<RET(A1,A2)> Func;
    typedef          RET Sig(A1,A2);
  };
  
  
  template< typename RET
          , typename A1
          , typename A2
          , typename A3
          >
  struct FunctionTypedef<RET, Types<A1,A2,A3>>
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
  struct FunctionTypedef<RET, Types<A1,A2,A3,A4>>
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
  struct FunctionTypedef<RET, Types<A1,A2,A3,A4,A5>>
  {
    typedef function<RET(A1,A2,A3,A4,A5)> Func;
    typedef          RET Sig(A1,A2,A3,A4,A5);
  };
  
  
  template< typename RET
          , typename A1
          , typename A2
          , typename A3
          , typename A4
          , typename A5
          , typename A6
          >
  struct FunctionTypedef<RET, Types<A1,A2,A3,A4,A5,A6>>
  {
    typedef function<RET(A1,A2,A3,A4,A5,A6)> Func;
    typedef          RET Sig(A1,A2,A3,A4,A5,A6);
  };
  
  
  template< typename RET
          , typename A1
          , typename A2
          , typename A3
          , typename A4
          , typename A5
          , typename A6
          , typename A7
          >
  struct FunctionTypedef<RET, Types<A1,A2,A3,A4,A5,A6,A7>>
  {
    typedef function<RET(A1,A2,A3,A4,A5,A6,A7)> Func;
    typedef          RET Sig(A1,A2,A3,A4,A5,A6,A7);
  };
  
  
  template< typename RET
          , typename A1
          , typename A2
          , typename A3
          , typename A4
          , typename A5
          , typename A6
          , typename A7
          , typename A8
          >
  struct FunctionTypedef<RET, Types<A1,A2,A3,A4,A5,A6,A7,A8>>
  {
    typedef function<RET(A1,A2,A3,A4,A5,A6,A7,A8)> Func;
    typedef          RET Sig(A1,A2,A3,A4,A5,A6,A7,A8);
  };
  
  
  template< typename RET
          , typename A1
          , typename A2
          , typename A3
          , typename A4
          , typename A5
          , typename A6
          , typename A7
          , typename A8
          , typename A9
          >
  struct FunctionTypedef<RET, Types<A1,A2,A3,A4,A5,A6,A7,A8,A9>>
  {
    typedef function<RET(A1,A2,A3,A4,A5,A6,A7,A8,A9)> Func;
    typedef          RET Sig(A1,A2,A3,A4,A5,A6,A7,A8,A9);
  };
  
  
  
  
  
}} // namespace lib::meta
#endif
