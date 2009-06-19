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
  
  
  
  namespace func { // helpers for binding and applying a function to an argument tuple 
    
    using tuple::element;
    
    /** 
     * this Helper with repetitive specialisations for up to nine arguments
     * is used either to apply a function to arguments given as a tuple, or
     * to create the actual closure (functor) over all function arguments. 
     */
    template<uint n>
    struct Apply;
    
    
    template<>                                            //_________________________________
    struct Apply<1>                                      ///< Apply function with 1 Argument
      {
        template<class FUN, typename RET, class TUP>
        static RET
        invoke (FUN f, TUP & arg)
          {
            return f (element<1>(arg));
          }
        
        template<class FUN, typename RET, class TUP>
        static RET
        bind (FUN f, TUP & arg)
          {
            return std::tr1::bind (f, element<1>(arg));
          }
      };
    
    
    template<>                                            //_________________________________
    struct Apply<2>                                      ///< Apply function with 2 Arguments
      {
        template<class FUN, typename RET, class TUP>
        static RET
        invoke (FUN f, TUP & arg)
          {
            return f ( element<1>(arg)
                     , element<2>(arg)
                     ); 
          }
        
        template<class FUN, typename RET, class TUP>
        static RET
        bind (FUN f, TUP & arg)
          {
            return std::tr1::bind (f, element<1>(arg)
                                    , element<2>(arg)
                                   );
          }
      };
    
    
    template<>                                            //_________________________________
    struct Apply<3>                                      ///< Apply function with 3 Arguments
      {
        template<class FUN, typename RET, class TUP>
        static RET
        invoke (FUN f, TUP & arg)
          {
            return f ( element<1>(arg)
                     , element<2>(arg)
                     , element<3>(arg)
                     ); 
          }
        
        template<class FUN, typename RET, class TUP>
        static RET
        bind (FUN f, TUP & arg)
          {
            return std::tr1::bind (f, element<1>(arg)
                                    , element<2>(arg)
                                    , element<3>(arg)
                                   );
          }
      };
    
    
    template<>                                            //_________________________________
    struct Apply<4>                                      ///< Apply function with 4 Arguments
      {
        template<class FUN, typename RET, class TUP>
        static RET
        invoke (FUN f, TUP & arg)
          {
            return f ( element<1>(arg)
                     , element<2>(arg)
                     , element<3>(arg)
                     , element<4>(arg)
                     ); 
          }
        
        template<class FUN, typename RET, class TUP>
        static RET
        bind (FUN f, TUP & arg)
          {
            return std::tr1::bind (f, element<1>(arg)
                                    , element<2>(arg)
                                    , element<3>(arg)
                                    , element<4>(arg)
                                   );
          }
      };
    
    
    template<>                                            //_________________________________
    struct Apply<5>                                      ///< Apply function with 5 Arguments
      {
        template<class FUN, typename RET, class TUP>
        static RET
        invoke (FUN f, TUP & arg)
          {
            return f ( element<1>(arg)
                     , element<2>(arg)
                     , element<3>(arg)
                     , element<4>(arg)
                     , element<5>(arg)
                     ); 
          }
        
        template<class FUN, typename RET, class TUP>
        static RET
        bind (FUN f, TUP & arg)
          {
            return std::tr1::bind (f, element<1>(arg)
                                    , element<2>(arg)
                                    , element<3>(arg)
                                    , element<4>(arg)
                                    , element<5>(arg)
                                   );
          }
      };
    
    
    template<>                                            //_________________________________
    struct Apply<6>                                      ///< Apply function with 6 Arguments
      {
        template<class FUN, typename RET, class TUP>
        static RET
        invoke (FUN f, TUP & arg)
          {
            return f ( element<1>(arg)
                     , element<2>(arg)
                     , element<3>(arg)
                     , element<4>(arg)
                     , element<5>(arg)
                     , element<6>(arg)
                     ); 
          }
        
        template<class FUN, typename RET, class TUP>
        static RET
        bind (FUN f, TUP & arg)
          {
            return std::tr1::bind (f, element<1>(arg)
                                    , element<2>(arg)
                                    , element<3>(arg)
                                    , element<4>(arg)
                                    , element<5>(arg)
                                    , element<6>(arg)
                                   );
          }
      };
    
    
    template<>                                            //_________________________________
    struct Apply<7>                                      ///< Apply function with 7 Arguments
      {
        template<class FUN, typename RET, class TUP>
        static RET
        invoke (FUN f, TUP & arg)
          {
            return f ( element<1>(arg)
                     , element<2>(arg)
                     , element<3>(arg)
                     , element<4>(arg)
                     , element<5>(arg)
                     , element<6>(arg)
                     , element<7>(arg)
                     ); 
          }
        
        template<class FUN, typename RET, class TUP>
        static RET
        bind (FUN f, TUP & arg)
          {
            return std::tr1::bind (f, element<1>(arg)
                                    , element<2>(arg)
                                    , element<3>(arg)
                                    , element<4>(arg)
                                    , element<5>(arg)
                                    , element<6>(arg)
                                    , element<7>(arg)
                                   );
          }
      };
    
    
    template<>                                            //_________________________________
    struct Apply<8>                                      ///< Apply function with 8 Arguments
      {
        template<class FUN, typename RET, class TUP>
        static RET
        invoke (FUN f, TUP & arg)
          {
            return f ( element<1>(arg)
                     , element<2>(arg)
                     , element<3>(arg)
                     , element<4>(arg)
                     , element<5>(arg)
                     , element<6>(arg)
                     , element<7>(arg)
                     , element<8>(arg)
                     ); 
          }
        
        template<class FUN, typename RET, class TUP>
        static RET
        bind (FUN f, TUP & arg)
          {
            return std::tr1::bind (f, element<1>(arg)
                                    , element<2>(arg)
                                    , element<3>(arg)
                                    , element<4>(arg)
                                    , element<5>(arg)
                                    , element<6>(arg)
                                    , element<7>(arg)
                                    , element<8>(arg)
                                   );
          }
      };
    
    
    template<>                                            //_________________________________
    struct Apply<9>                                      ///< Apply function with 9 Arguments
      {
        template<class FUN, typename RET, class TUP>
        static RET
        invoke (FUN f, TUP & arg)
          {
            return f ( element<1>(arg)
                     , element<2>(arg)
                     , element<3>(arg)
                     , element<4>(arg)
                     , element<5>(arg)
                     , element<6>(arg)
                     , element<7>(arg)
                     , element<8>(arg)
                     , element<9>(arg)
                     ); 
          }
        
        template<class FUN, typename RET, class TUP>
        static RET
        bind (FUN f, TUP & arg)
          {
            return std::tr1::bind (f, element<1>(arg)
                                    , element<2>(arg)
                                    , element<3>(arg)
                                    , element<4>(arg)
                                    , element<5>(arg)
                                    , element<6>(arg)
                                    , element<7>(arg)
                                    , element<8>(arg)
                                    , element<9>(arg)
                                   );
          }
      };
    
  } // (END) impl-namespace (func)
  
  
  
  /**
   * Closure-creating template.
   */
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
      
      function<SIG>  bind (SIG& f)                 { return func::Apply<ARG_CNT>::bind (f, params_); }
      function<SIG>  bind (function<SIG> const& f) { return func::Apply<ARG_CNT>::bind (f, params_); }
      
      Ret      operator() (SIG& f)                 { return func::Apply<ARG_CNT>::invoke (f, params_); }
      Ret      operator() (function<SIG> const& f) { return func::Apply<ARG_CNT>::invoke (f, params_); }
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
      
      typedef Ret  result_type;  ///< for STL use
      typedef void argument_type;
    };
  
  
  
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
