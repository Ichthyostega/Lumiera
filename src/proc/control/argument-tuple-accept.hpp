/*
  ARGUMENT-TUPLE-ACCEPT.hpp  -  helper template providing a bind(...) member function

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


/** @file argument-tuple-accept.hpp
 ** Mixin-templates providing arbitrary function call operators and argument
 ** binding functions. By inheriting from one of these templates, a class can
 ** accept a specifically typed binding or function call, as specified by the
 ** template parameters, or alternatively it can inherit a complete set of
 ** templated argument binding functions, assuming that the matching signature
 ** can be detected at runtime. These templates are used for the Proc-Layer
 ** command frontend, to bind to the actual command arguments.
 **   
 ** The AcceptArgumentBinding template allows to mix in a \c bind(...) function.
 ** Thereby, the correct number and types of arguments is derived according to
 ** the function signature given as template parameter. The class mixing in this
 ** template needs to provide a suitable member function \c bindArg(Tuple<...>) ,
 ** which accepts all the command arguments packaged together into a tuple (record).
 ** AcceptArgumentTuple works similar, but provides function call operators rather.
 ** 
 ** Contrary to this, the AcceptAnyBinding mixin template provides a complete
 ** set of \c bind(...) functions, accepting up to 9 arbitrary call parameters and
 ** again forwarding the call to a template member function \c bindArg(Tuple<...>)
 ** This helper template is used on the control::Command frontend objects; in this
 ** case, there is a runtime type-check built into CommandImpl which will throw
 ** when the provided arguments don't fit the (hidden) function signature embedded
 ** within the CommandMutation (functor).
 ** 
 ** @see Command
 ** @see CommandDef
 ** @see argument-tuple-accept-test.cpp
 ** @see command-invocation.hpp
 **
 */



#ifndef CONTROL_ARGUMENT_TUPLE_ACCEPT_H
#define CONTROL_ARGUMENT_TUPLE_ACCEPT_H

#include "lib/meta/typelist.hpp"
#include "lib/meta/function.hpp"
#include "lib/meta/tuple-helper.hpp"

#include <tuple>



namespace proc {
namespace control {
  
  
  namespace bind_arg { // internals....
    
    using namespace lib::meta;
    using std::make_tuple;
    
    
      //
     // _______________________________________________________________________________________________________________
    /** @internal mix in a function operator */
    template< class TAR, class BA, class RET
            , typename TYPES
            >
    struct AcceptArgs ;
    
    
    /* specialisations for 0...9 Arguments.... */
    
    template< class TAR, class BA, class RET
            >                                                                        //____________________________________
    struct AcceptArgs<TAR,BA,RET, Types<> >                                         ///< Accept dummy binding (0 Arguments)
      : BA
      {
        RET
        operator() ()
          {
            return static_cast<TAR*> (this) -> bindArg (std::tuple<>() );
          }
      };
    
    
    template< class TAR, class BA, class RET
            , typename T1
            >                                                                        //_______________________________
    struct AcceptArgs<TAR,BA,RET, Types<T1> >                                       ///< Accept binding for 1 Argument
      : BA
      {
        RET
        operator() (T1 a1)
          {
            return static_cast<TAR*> (this) -> bindArg (make_tuple (a1));
          }
      };
    
    
    template< class TAR, class BA, class RET
            , typename T1
            , typename T2
            >                                                                        //________________________________
    struct AcceptArgs<TAR,BA,RET, Types<T1,T2> >                                    ///< Accept binding for 2 Arguments
      : BA
      {
        RET
        operator() (T1 a1, T2 a2)
          {
            return static_cast<TAR*> (this) -> bindArg (make_tuple (a1,a2));
          }
      };
    
    
    template< class TAR, class BA, class RET
            , typename T1
            , typename T2
            , typename T3
            >                                                                        //________________________________
    struct AcceptArgs<TAR,BA,RET, Types<T1,T2,T3> >                                 ///< Accept binding for 3 Arguments
      : BA
      {
        RET
        operator() (T1 a1, T2 a2, T3 a3)
          {
            return static_cast<TAR*> (this) -> bindArg (make_tuple (a1,a2,a3));
          }
      };
    
    
    template< class TAR, class BA, class RET
            , typename T1
            , typename T2
            , typename T3
            , typename T4
            >                                                                        //________________________________
    struct AcceptArgs<TAR,BA,RET, Types<T1,T2,T3,T4> >                              ///< Accept binding for 4 Arguments
      : BA
      {
        RET
        operator() (T1 a1, T2 a2, T3 a3, T4 a4)
          {
            return static_cast<TAR*> (this) -> bindArg (make_tuple (a1,a2,a3,a4));
          }
      };
    
    
    template< class TAR, class BA, class RET
            , typename T1
            , typename T2
            , typename T3
            , typename T4
            , typename T5
            >                                                                        //________________________________
    struct AcceptArgs<TAR,BA,RET, Types<T1,T2,T3,T4,T5> >                           ///< Accept binding for 5 Arguments
      : BA
      {
        RET
        operator() (T1 a1, T2 a2, T3 a3, T4 a4, T5 a5)
          {
            return static_cast<TAR*> (this) -> bindArg (make_tuple (a1,a2,a3,a4,a5));
          }
      };
    
    
    template< class TAR, class BA, class RET
            , typename T1
            , typename T2
            , typename T3
            , typename T4
            , typename T5
            , typename T6
            >                                                                        //________________________________
    struct AcceptArgs<TAR,BA,RET, Types<T1,T2,T3,T4,T5,T6> >                        ///< Accept binding for 6 Arguments
      : BA
      {
        RET
        operator() (T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6)
          {
            return static_cast<TAR*> (this) -> bindArg (make_tuple (a1,a2,a3,a4,a5,a6));
          }
      };
    
    
    template< class TAR, class BA, class RET
            , typename T1
            , typename T2
            , typename T3
            , typename T4
            , typename T5
            , typename T6
            , typename T7
            >                                                                        //________________________________
    struct AcceptArgs<TAR,BA,RET, Types<T1,T2,T3,T4,T5,T6,T7> >                     ///< Accept binding for 7 Arguments
      : BA
      {
        RET
        operator() (T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6, T7 a7)
          {
            return static_cast<TAR*> (this) -> bindArg (make_tuple (a1,a2,a3,a4,a5,a6,a7));
          }
      };
    
    
    template< class TAR, class BA, class RET
            , typename T1
            , typename T2
            , typename T3
            , typename T4
            , typename T5
            , typename T6
            , typename T7
            , typename T8
            >                                                                        //________________________________
    struct AcceptArgs<TAR,BA,RET, Types<T1,T2,T3,T4,T5,T6,T7,T8> >                  ///< Accept binding for 8 Arguments
      : BA
      {
        RET
        operator() (T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6, T7 a7, T8 a8)
          {
            return static_cast<TAR*> (this) -> bindArg (make_tuple (a1,a2,a3,a4,a5,a6,a7,a8));
          }
      };
    
    
    template< class TAR, class BA, class RET
            , typename T1
            , typename T2
            , typename T3
            , typename T4
            , typename T5
            , typename T6
            , typename T7
            , typename T8
            , typename T9
            >                                                                        //________________________________
    struct AcceptArgs<TAR,BA,RET, Types<T1,T2,T3,T4,T5,T6,T7,T8,T9> >               ///< Accept binding for 9 Arguments
      : BA
      {
        RET
        operator() (T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6, T7 a7, T8 a8, T9 a9)
          {
            return static_cast<TAR*> (this) -> bindArg (make_tuple (a1,a2,a3,a4,a5,a6,a7,a8,a9));
          }
      };
    
    
    
    
    
    
      //
     // _______________________________________________________________________________________________________________
    /** @internal mix in a \c bind() function
     */
    template< class TAR, class BA, class RET
            , typename TYPES
            >
    struct AcceptBind ;
    
    
    /* specialisations for 0...9 Arguments.... */
    
    template< class TAR, class BA, class RET
            >                                                                        //____________________________________
    struct AcceptBind<TAR,BA,RET, Types<> >                                         ///< Accept dummy binding (0 Arguments)
      : BA
      {
        RET
        bind ()
          {
            return static_cast<TAR*> (this) -> bindArg (std::tuple<>() );
          }
      };
    
    
    template< class TAR, class BA, class RET
            , typename T1
            >                                                                        //_______________________________
    struct AcceptBind<TAR,BA,RET, Types<T1> >                                       ///< Accept binding for 1 Argument
      : BA
      {
        RET
        bind (T1 a1)
          {
            return static_cast<TAR*> (this) -> bindArg (make_tuple (a1));
          }
      };
    
    
    template< class TAR, class BA, class RET
            , typename T1
            , typename T2
            >                                                                        //________________________________
    struct AcceptBind<TAR,BA,RET, Types<T1,T2> >                                    ///< Accept binding for 2 Arguments
      : BA
      {
        RET
        bind (T1 a1, T2 a2)
          {
            return static_cast<TAR*> (this) -> bindArg (make_tuple (a1,a2));
          }
      };
    
    
    template< class TAR, class BA, class RET
            , typename T1
            , typename T2
            , typename T3
            >                                                                        //________________________________
    struct AcceptBind<TAR,BA,RET, Types<T1,T2,T3> >                                 ///< Accept binding for 3 Arguments
      : BA
      {
        RET
        bind (T1 a1, T2 a2, T3 a3)
          {
            return static_cast<TAR*> (this) -> bindArg (make_tuple (a1,a2,a3));
          }
      };
    
    
    template< class TAR, class BA, class RET
            , typename T1
            , typename T2
            , typename T3
            , typename T4
            >                                                                        //________________________________
    struct AcceptBind<TAR,BA,RET, Types<T1,T2,T3,T4> >                              ///< Accept binding for 4 Arguments
      : BA
      {
        RET
        bind (T1 a1, T2 a2, T3 a3, T4 a4)
          {
            return static_cast<TAR*> (this) -> bindArg (make_tuple (a1,a2,a3,a4));
          }
      };
    
    
    template< class TAR, class BA, class RET
            , typename T1
            , typename T2
            , typename T3
            , typename T4
            , typename T5
            >                                                                        //________________________________
    struct AcceptBind<TAR,BA,RET, Types<T1,T2,T3,T4,T5> >                           ///< Accept binding for 5 Arguments
      : BA
      {
        RET
        bind (T1 a1, T2 a2, T3 a3, T4 a4, T5 a5)
          {
            return static_cast<TAR*> (this) -> bindArg (make_tuple (a1,a2,a3,a4,a5));
          }
      };
    
    
    template< class TAR, class BA, class RET
            , typename T1
            , typename T2
            , typename T3
            , typename T4
            , typename T5
            , typename T6
            >                                                                        //________________________________
    struct AcceptBind<TAR,BA,RET, Types<T1,T2,T3,T4,T5,T6> >                        ///< Accept binding for 6 Arguments
      : BA
      {
        RET
        bind (T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6)
          {
            return static_cast<TAR*> (this) -> bindArg (make_tuple (a1,a2,a3,a4,a5,a6));
          }
      };
    
    
    template< class TAR, class BA, class RET
            , typename T1
            , typename T2
            , typename T3
            , typename T4
            , typename T5
            , typename T6
            , typename T7
            >                                                                        //________________________________
    struct AcceptBind<TAR,BA,RET, Types<T1,T2,T3,T4,T5,T6,T7> >                     ///< Accept binding for 7 Arguments
      : BA
      {
        RET
        bind (T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6, T7 a7)
          {
            return static_cast<TAR*> (this) -> bindArg (make_tuple (a1,a2,a3,a4,a5,a6,a7));
          }
      };
    
    
    template< class TAR, class BA, class RET
            , typename T1
            , typename T2
            , typename T3
            , typename T4
            , typename T5
            , typename T6
            , typename T7
            , typename T8
            >                                                                        //________________________________
    struct AcceptBind<TAR,BA,RET, Types<T1,T2,T3,T4,T5,T6,T7,T8> >                  ///< Accept binding for 8 Arguments
      : BA
      {
        RET
        bind (T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6, T7 a7, T8 a8)
          {
            return static_cast<TAR*> (this) -> bindArg (make_tuple (a1,a2,a3,a4,a5,a6,a7,a8));
          }
      };
    
    
    template< class TAR, class BA, class RET
            , typename T1
            , typename T2
            , typename T3
            , typename T4
            , typename T5
            , typename T6
            , typename T7
            , typename T8
            , typename T9
            >                                                                        //________________________________
    struct AcceptBind<TAR,BA,RET, Types<T1,T2,T3,T4,T5,T6,T7,T8,T9> >               ///< Accept binding for 9 Arguments
      : BA
      {
        RET
        bind (T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6, T7 a7, T8 a8, T9 a9)
          {
            return static_cast<TAR*> (this) -> bindArg (make_tuple (a1,a2,a3,a4,a5,a6,a7,a8,a9));
          }
      };
    
    
    
    
    
    
      //
     // _______________________________________________________________________________________________________________
    /** @internal mix in complete set of templated \c bind() functions
     */
    template< class TAR, class BA, class RET>
    struct AcceptAnyBind
      : BA
      {
        
        RET                                                                          //________________________________
        bind ()                                                                     ///< Accept dummy binding (0 Arg)
          {
            return static_cast<TAR*> (this) -> bindArg (std::tuple<>());
          }
        
        
        template<typename T1>
        RET                                                                          //________________________________
        bind (T1 a1)                                                                ///< Accept binding with 1 Argument
          {
            return static_cast<TAR*> (this) -> bindArg (make_tuple (a1));
          }
        
        
        template< typename T1
                , typename T2
                >
        RET                                                                          //________________________________
        bind (T1 a1, T2 a2)                                                         ///< Accept binding for 2 Arguments
          {
            return static_cast<TAR*> (this) -> bindArg (make_tuple (a1,a2));
          }
        
        
        template< typename T1
                , typename T2
                , typename T3
                >
        RET                                                                          //________________________________
        bind (T1 a1, T2 a2, T3 a3)                                                  ///< Accept binding for 3 Arguments
          {
            return static_cast<TAR*> (this) -> bindArg (make_tuple (a1,a2,a3));
          }
        
        
        template< typename T1
                , typename T2
                , typename T3
                , typename T4
                >
        RET                                                                          //________________________________
        bind (T1 a1, T2 a2, T3 a3, T4 a4)                                           ///< Accept binding for 4 Arguments
          {
            return static_cast<TAR*> (this) -> bindArg (make_tuple (a1,a2,a3,a4));
          }
        
        
        template< typename T1
                , typename T2
                , typename T3
                , typename T4
                , typename T5
                >
        RET                                                                          //________________________________
        bind (T1 a1, T2 a2, T3 a3, T4 a4, T5 a5)                                    ///< Accept binding for 5 Arguments
          {
            return static_cast<TAR*> (this) -> bindArg (make_tuple (a1,a2,a3,a4,a5));
          }
        
        
        template< typename T1
                , typename T2
                , typename T3
                , typename T4
                , typename T5
                , typename T6
                >
        RET                                                                          //________________________________
        bind (T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6)                             ///< Accept binding for 6 Arguments
          {
            return static_cast<TAR*> (this) -> bindArg (make_tuple (a1,a2,a3,a4,a5,a6));
          }
        
        
        template< typename T1
                , typename T2
                , typename T3
                , typename T4
                , typename T5
                , typename T6
                , typename T7
                >
        RET                                                                          //________________________________
        bind (T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6, T7 a7)                      ///< Accept binding for 7 Arguments
          {
            return static_cast<TAR*> (this) -> bindArg (make_tuple (a1,a2,a3,a4,a5,a6,a7));
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
        RET                                                                          //________________________________
        bind (T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6, T7 a7, T8 a8)               ///< Accept binding for 8 Arguments
          {
            return static_cast<TAR*> (this) -> bindArg (make_tuple (a1,a2,a3,a4,a5,a6,a7,a8));
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
        RET                                                                          //________________________________
        bind (T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6, T7 a7, T8 a8, T9 a9)        ///< Accept binding for 9 Arguments
          {
            return static_cast<TAR*> (this) -> bindArg (make_tuple (a1,a2,a3,a4,a5,a6,a7,a8,a9));
          }
        
      };
    
    
    using lib::meta::Tuple;
    
    
    template<typename SIG>
    struct _Type
      {
        using Args = typename FunctionSignature< function<SIG> >::Args;
        using Ret  = typename FunctionSignature< function<SIG> >::Ret;
        using Sig  = SIG;
        using ArgTuple = Tuple<Args>;
      };
    
    template<typename...TYPES>
    struct _Type<std::tuple<TYPES...> >
      {
        using Args = typename Types<TYPES...>::Seq;
        using Ret  = void;
        using Sig  = typename FunctionTypedef<void, Args>::Sig;
        using ArgTuple = std::tuple<TYPES...>;
      };
  
    struct Dummy {}; 
    
    
  } // (END) impl details (bind_arg)
  
  
  
  
  /** Helper Template for building a Functor or function-like class:
   *  Mix in a function call operator, which mimics the specified signature SIG .
   *  This template is to be used as a base class to inherit the target type TAR from;
   *  this target type is assumed to provide a function \bindArg(Tuple<TYPES..>) --
   *  where \c TYPES... is the sequence of types found in the provided Signature SIG.
   */
  template<typename SIG, class TAR, class BASE =bind_arg::Dummy>
  class AcceptArgumentTuple
    : public bind_arg::AcceptArgs<TAR,BASE, typename bind_arg::_Type<SIG>::Ret
                                          , typename bind_arg::_Type<SIG>::Args>
    { };
  
  
  /** Helper Template for Proc-Layer control::Command : mix in a \c bind(...) function
   *  @param SIG  function signature to mimic (regarding the arguments and return type)
   *  @param TAR  the target class providing a function \c bindArg(Tuple<Types<T1...> >)
   *  @param BASE the base class for inheritance chaining
   */
  template<typename SIG, class TAR, class BASE =bind_arg::Dummy>
  class AcceptArgumentBinding
    : public bind_arg::AcceptBind<TAR,BASE, typename bind_arg::_Type<SIG>::Ret
                                          , typename bind_arg::_Type<SIG>::Args>   
    { };
  
    
  /** Variation of AcceptArgumentBinding, allowing to control the return type 
   *  of the generated \c bind(...) functions independently from SIG
   */
  template<typename RET, typename SIG, class TAR, class BASE =bind_arg::Dummy>
  class AcceptArgumentBindingRet
    : public bind_arg::AcceptBind<TAR,BASE, RET
                                          , typename bind_arg::_Type<SIG>::Args>
    { };
  
  
  /** Helper Template for control::Command, mix-in complete set of \c bind(...) functions
   *  @param TAR  the target class providing a function \c bindArg(Tuple<Types<T1...> >)
   *  @param RET  common return type of \c bindArg() and all \c bind() functions  
   *  @param BASE the base class for inheritance chaining
   */
  template< class TAR
          , class RET  =TAR&
          , class BASE =bind_arg::Dummy
          >
  class AcceptAnyBinding
    : public bind_arg::AcceptAnyBind<TAR,BASE,RET>
    { };
  
  
  
  
  
  
}} // namespace proc::control
#endif
