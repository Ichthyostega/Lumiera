/*
  FUNCTION-ERASURE.hpp  -  wrapping a functor object for inline storage while hiding the signature
 
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


/** @file function-erasure.hpp
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


#ifndef LUMIERA_META_FUNCTION_ERASURE_H
#define LUMIERA_META_FUNCTION_ERASURE_H

#include "lib/meta/typelist.hpp"   /////////////TODO
#include "lib/meta/generator.hpp"  /////////////TODO
#include "lib/meta/function.hpp"
#include "lib/meta/tuple.hpp"

#include <tr1/functional>

#include "lib/util.hpp" ////////////////////////TODO
#include "lib/error.hpp"


namespace lumiera {
namespace typelist{

  using std::tr1::bind;
  //using std::tr1::placeholders::_1;
  //using std::tr1::placeholders::_2;
  using std::tr1::function;
  
  
  
  /**
   * Generic wrapper carrying a function object
   * while hiding the actual function signature
   * @note not statically typesafe. Depending on
   *       the actual embedded container type,
   *       it \em might be run-time typesafe.
   */
  template<class FH>
  struct FunErasure
    : FH
    {
      template<typename FUN>
      FunErasure (FUN const& functor)
        : FH(functor)
        { }
    };
    
  /** 
   * Policy for FunErasure: store an embedded tr1::function
   * Using this policy allows to store arbitrary complex functor objects
   * embedded within a neutral container and retrieving them later type-safe.
   * The price to pay is vtable access and heap storage of function arguments. 
   */
  class StoreFunction
    {
      /** Helper: type erasure */
      struct Holder
        {
          enum { SIZE = sizeof(function<void(void)>) };
          char storage_[SIZE];
          virtual ~Holder() {}
        };
      
      /** embedding the concrete functor object */
      template<typename SIG>
      struct FunctionHolder : Holder
        {
          typedef function<SIG> Functor;
          FunctionHolder (SIG& fun)
            {
              REQUIRE (SIZE >= sizeof(Functor));
              new(&storage_) Functor (fun);
            }
          ~FunctionHolder()
            {
              get()->~Functor();
            }
          Functor&
          get()
            {
              return static_cast<Functor*> (&storage_);
            }
        };
      
      /** embedded buffer actually holding
       *  the concrete Functor object */
      Holder holder_;
      
    public:
      template<typename SIG>
      StoreFunction (SIG& fun)
        {
          new(&holder_) FunctionHolder<SIG> (fun);
        }
      
      template<typename SIG>
      function<SIG>&
      getFun ()
        {
          REQUIRE (INSTANCEOF (FunctionHolder<SIG>, &holder_));
          return static_cast<FunctionHolder<SIG>&> (holder_).get();
        }
    };
  
  
  /** 
   * Policy for FunErasure: store a bare function pointer.
   * Using this policy allows to store a conventional function ptr,
   * while still being able to re-access it later with run-time typecheck.
   * The price to pay is vtable access. 
   */
  class StoreFunPtr
    {
      /** Helper: type erasure */
      struct Holder
        {
          void *fun_;
          virtual ~Holder() {}
        };
      
      /** storing and retrieving concrete function ptr */
      template<typename SIG>
      struct FunctionHolder : Holder
        {
          FunctionHolder (SIG& fun)
            {
              fun_ = &fun;
            }
          SIG&
          get()
            {
              return reinterpret_cast<SIG*> (&fun_);
            }
        };
      
      /** embedded container holding the pointer */
      Holder holder_;
      
    public:
      template<typename SIG>
      StoreFunPtr (SIG& fun)
        {
          new(&holder_) FunctionHolder<SIG> (fun);
        }
      
      template<typename SIG>
      function<SIG>&
      getFun ()
        {
          REQUIRE (INSTANCEOF (FunctionHolder<SIG>, &holder_));
          return static_cast<FunctionHolder<SIG>&> (holder_).get();
        }
    };
  
  
  /** 
   * Policy for FunErasure: store an unchecked bare function pointer.
   * Using this policy allows to store a conventional function ptr,
   * and to retrieve it without overhead, but also without safety.
   */
  class StoreUncheckedFunPtr
    {
      void *fun_;
      
    public:
      template<typename SIG>
      StoreUncheckedFunPtr (SIG& fun)
        {
          fun_ = &fun;
        }
      
      template<typename SIG>
      SIG&
      getFun ()
        {
          return reinterpret_cast<SIG*> (&fun_);
        }
    };
  
  
  
  
}} // namespace lumiera::typelist
#endif
