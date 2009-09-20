/*
  COMMAND-BINDING.hpp  -  interface for various ways of binding command arguments
 
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


/** @file command-binding.hpp
 ** Sub-include of command.hpp, providing an interface to various ways
 ** of binding command arguments. While in some cases, a command will be
 ** set up completely finished and closed over all it's arguments, usually
 ** the CommandDef will just specify the command operation and undo function,
 ** thus leaving the task of binding concrete arguments to the client code.
 ** Thus, depending on the circumstances, there might be usage situations
 ** where the exact number and type of arguments can be detected and checked
 ** at compile time, while otherwise this check needs to be deferred to happen
 ** at runtime, when the binding is actually invoked.
 **
 ** @see Command
 ** @see CommandDef
 ** @see command-use1-test.cpp
 **
 */



#ifndef CONTROL_COMMAND_BINDING_H
#define CONTROL_COMMAND_BINDING_H

#include "lib/meta/typelist.hpp"
#include "lib/meta/tuple.hpp"




namespace control {
namespace com { ///< Proc-Layer command implementation details
  
  using namespace lumiera::typelist;
  
  
  
////////////////////////TODO: Ticket #266  - can this be merged into argument-tuple-accept.hpp ??
  
  
  /**
   * Building block for commands, allowing to mix in
   * a set of \c bind(...) function for up to nine arbitrary arguments.
   * All these functions will package the argument values into a Tuple
   * (record) and forward the call to a \c bindArg(Tuple<TYPES...>) function
   * on the target class. The latter function is assumed to perform a 
   * run-time check to detect calls with invalid signature.
   */
  template<class TAR, class BA>
  class ArgumentBinder
    : public BA
    {
      
    public:
    
    
    /** Arm up a command by binding it with concrete arguments.
     *  At this point, a run time type check is performed, to find out
     *  if the number and type of arguments of the stored operation function
     *  within the command matches the given argument pattern
     */
    TAR&
    bind ()
      {
        return static_cast<TAR*> (this) -> bindArg (tuple::makeNullTuple());
      }
    
    
    template<typename T1>
    TAR&                                                                                  //________________________________
    bind (T1 a1)                                                                         ///< Accept binding with 1 Argument
      {
        return static_cast<TAR*> (this) -> bindArg (tuple::make (a1));
      }
    
    
    template< typename T1
            , typename T2
            >
    TAR&                                                                                  //________________________________
    bind (T1 a1, T2 a2)                                                                  ///< Accept binding for 2 Arguments
      {
        return static_cast<TAR*> (this) -> bindArg (tuple::make (a1,a2));
      }
    
    
    template< typename T1
            , typename T2
            , typename T3
            >
    TAR&                                                                                  //________________________________
    bind (T1 a1, T2 a2, T3 a3)                                                           ///< Accept binding for 3 Arguments
      {
        return static_cast<TAR*> (this) -> bindArg (tuple::make (a1,a2,a3));
      }
    
    
    template< typename T1
            , typename T2
            , typename T3
            , typename T4
            >
    TAR&                                                                                  //________________________________
    bind (T1 a1, T2 a2, T3 a3, T4 a4)                                                    ///< Accept binding for 4 Arguments
      {
        return static_cast<TAR*> (this) -> bindArg (tuple::make (a1,a2,a3,a4));
      }
    
    
    template< typename T1
            , typename T2
            , typename T3
            , typename T4
            , typename T5
            >
    TAR&                                                                                  //________________________________
    bind (T1 a1, T2 a2, T3 a3, T4 a4, T5 a5)                                             ///< Accept binding for 5 Arguments
      {
        return static_cast<TAR*> (this) -> bindArg (tuple::make (a1,a2,a3,a4,a5));
      }
    
    
    template< typename T1
            , typename T2
            , typename T3
            , typename T4
            , typename T5
            , typename T6
            >
    TAR&                                                                                  //________________________________
    bind (T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6)                                      ///< Accept binding for 6 Arguments
      {
        return static_cast<TAR*> (this) -> bindArg (tuple::make (a1,a2,a3,a4,a5,a6));
      }
    
    
    template< typename T1
            , typename T2
            , typename T3
            , typename T4
            , typename T5
            , typename T6
            , typename T7
            >
    TAR&                                                                                  //________________________________
    bind (T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6, T7 a7)                               ///< Accept binding for 7 Arguments
      {
        return static_cast<TAR*> (this) -> bindArg (tuple::make (a1,a2,a3,a4,a5,a6,a7));
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
    TAR&                                                                                  //________________________________
    bind (T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6, T7 a7, T8 a8)                        ///< Accept binding for 8 Arguments
      {
        return static_cast<TAR*> (this) -> bindArg (tuple::make (a1,a2,a3,a4,a5,a6,a7,a8));
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
    TAR&                                                                                  //________________________________
    bind (T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6, T7 a7, T8 a8, T9 a9)                 ///< Accept binding for 9 Arguments
      {
        return static_cast<TAR*> (this) -> bindArg (tuple::make (a1,a2,a3,a4,a5,a6,a7,a8,a9));
      }
    
    };
  
  
  
  
}} // namespace control::com
#endif
