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
 ** When working with generic function objects and function pointers typed to
 ** arbitrary signatures, often there is the necessity to hold onto such a functor
 ** while hiding the actual signature behind an common interface ("type erasure").
 ** The usual solution based on subclassing has the downside of requiring separate
 ** storage for the concrete functor object, which might become problematic when
 ** dealing with lots of functor objects. 
 ** 
 ** Especially when dealing with tr1::function objects, all of the type differences
 ** are actually encoded into 3 internal pointers, thus yielding the same size for
 ** all various types of functors. Building on this observation, we can create an
 ** common container object to store the varying functors inline, while hiding the
 ** actual signature.
 ** 
 ** There remains the problem of re-accessing the concrete functor later on. As
 ** C++ has only rudimental introspection capabilities, we can only rely on the
 ** usage context to provide the correct function signature; only when using a
 ** virtual function for the re-access, we can perform at least a runtime-check.
 ** 
 ** Thus there are various flavours for actually implementing this idea, and the
 ** picking a suitable implementation depends largely on the context. Thus we
 ** provide a common and expect the client code to pick an implementation policy.
 ** 
 ** @see control::Mutation usage example
 ** @see function-erasure-test.cpp
 ** 
 */


#ifndef LUMIERA_META_FUNCTION_ERASURE_H
#define LUMIERA_META_FUNCTION_ERASURE_H

#include "lib/util.hpp"
#include "lib/error.hpp"

#include <tr1/functional>


namespace lumiera {
namespace typelist{
  
  using std::tr1::function;
  
  
  
  /******************************************************
   * Generic wrapper carrying a function object
   * while hiding the actual function signature
   * - create it using a function ref or pointer
   * - the StoreFunction-policy also allows 
   *   creation based on an existing function object
   * - re-access the functor or function ref
   *   using the templated \c getFun()
   *   
   * @param FH policy to control the implementation.
   *        In most cases, you should use "StoreFunction"  
   * @note not statically typesafe. Depending on
   *       the specified policy, it \em might be
   *       run-time typesafe.
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
  
    
  /* ====== Policy classes ====== */

  template<class T>
  struct BoolCheckable
    {
      typedef bool (T::*ValidityCheck)()  const;
      typedef ValidityCheck _unspecified_bool_type;
      ValidityCheck isValid;
      
      BoolCheckable() : isValid (&T::isValid) {}
      
      /** implicit conversion to "bool" */ 
      operator _unspecified_bool_type()  const   ///< never throws
        {
          T const& obj = static_cast<T const&> (*this);
          return  (obj.*isValid)()? isValid : 0;
        }
      
      bool operator! ()  const ///< never throws
        {
          T const& obj = static_cast<T const&> (*this);
          return !(obj.*isValid)();
        }
      
    };
    
  /** 
   * Policy for FunErasure: store an embedded tr1::function
   * Using this policy allows to store arbitrary complex functor objects
   * embedded within a neutral container and retrieving them later type-safe.
   * The price to pay is vtable access and heap storage of function arguments. 
   */
  class StoreFunction
//    : public BoolCheckable<StoreFunction>
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
          
          FunctionHolder (Functor const& fun)
            {
              REQUIRE (SIZE >= sizeof(Functor));
              new(&storage_) Functor (fun);
            }
          ~FunctionHolder()
            {
              get().~Functor();
            }
          Functor&
          get()
            {
              return *reinterpret_cast<Functor*> (&storage_);
            }
        };
      
      /** embedded buffer actually holding
       *  the concrete Functor object */
      Holder holder_;
      
    public:
      template<typename SIG>
      StoreFunction (SIG& fun)
        {
          new(&holder_) FunctionHolder<SIG> (function<SIG>(fun));
        }
      template<typename SIG>
      StoreFunction (function<SIG> const& fun)
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
      
      bool
      isValid()  const
        {
          return reinterpret_cast<void*> (holder_.storage_[0]);
        }
    };
  
  
  /**
   * Policy for FunErasure: store a bare function pointer.
   * Using this policy allows to store a conventional function ptr,
   * while still being able to re-access it later with run-time type check.
   * The price to pay is vtable access. 
   */
  class StoreFunPtr
    {
      /** Helper: type erasure */
      struct Holder
        {
          void *fP_;
          virtual ~Holder() {}
        };
      
      /** storing and retrieving concrete function ptr */
      template<typename SIG>
      struct FunctionHolder : Holder
        {
          FunctionHolder (SIG *fun)
            {
              REQUIRE (fun);
              fP_ = reinterpret_cast<void*> (fun);
            }
          SIG&
          get()
            {
              return *reinterpret_cast<SIG*> (fP_);
            }
        };
      
      /** embedded container holding the pointer */
      Holder holder_;
      
    public:
      template<typename SIG>
      StoreFunPtr (SIG& fun)
        {
          new(&holder_) FunctionHolder<SIG> (&fun);
        }
      template<typename SIG>
      StoreFunPtr (SIG *fun)
        {
          new(&holder_) FunctionHolder<SIG> (fun);
        }
      
      template<typename SIG>
      SIG&
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
      void *funP_;
      
    public:
      template<typename SIG>
      StoreUncheckedFunPtr (SIG& fun)
        {
          funP_ = reinterpret_cast<void*> (&fun);
        }
      template<typename SIG>
      StoreUncheckedFunPtr (SIG *fun)
        {
          funP_ = reinterpret_cast<void*> (fun);
        }
      
      template<typename SIG>
      SIG&
      getFun ()
        {
          return *reinterpret_cast<SIG*> (funP_);
        }
    };
  
  
  
  
}} // namespace lumiera::typelist
#endif
