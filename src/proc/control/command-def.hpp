/*
  COMMAND-DEF.hpp  -  defining and binding a Proc-Layer command
 
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


/** @file command-def.hpp
 ** Actually defining a command and binding it to execution parameters.
 ** While the header command.hpp contains everything needed for executing and
 ** commands and referring to them, this more heavy-weight header is needed when
 ** \em defining the concrete operations to be encapsulated into a command. To
 ** create a command, you need to provide three functors (for the actual operation,
 ** the undo operation and for capturing undo state prior to invoking the operation).
 ** Besides, you provide a \em binding, thus creating a closue out of these three
 ** function objects and a set of actual parameters. This closure effectively is 
 ** the command, which in a last step can be either dispatched, stored or 
 ** invoked immediately.
 ** //TODO 
 **
 ** @see Command
 ** @see Mutation
 ** @see CommandClosure
 ** @see ProcDispatcher
 ** @see CommandBasic_test simple usage example
 **
 */



#ifndef CONTROL_COMMAND_DEF_H
#define CONTROL_COMMAND_DEF_H

//#include "pre.hpp"
#include "include/symbol.hpp"
#include "proc/control/command.hpp"
#include "proc/control/command-signature.hpp"
#include "proc/control/command-mutation.hpp"
#include "proc/control/command-closure.hpp"
#include "lib/meta/function.hpp"
#include "lib/meta/typelist.hpp"
#include "lib/meta/typelistutil.hpp"
#include "lib/meta/tuple.hpp"

//#include <tr1/memory>
#include <tr1/functional>




namespace control {
  
  using lumiera::Symbol;
//  using std::tr1::shared_ptr;
  using std::tr1::function;
  
  using lumiera::typelist::FunctionSignature;
  using lumiera::typelist::FunctionTypedef;
  using lumiera::typelist::Types;
//using lumiera::typelist::NullType;
  using lumiera::typelist::Tuple;
  
  
  
  
  /**
   * Helper class used solely for \em defining a Command-Object.
   * This technique is known as "fluent API", see http://en.wikipedia.org/wiki/Fluent_interface
   * The basic idea is for the user to create a disposable instance of this definition helper,
   * only for calling a chain of definition functions, which internally build the actual Command object.
   * Finally, the created Command object will be stored into a registry or handed over to the
   * ProcDispatcher. To give an example:
   * \code
   *    CommandDefinition ("test.command1")
   *           .operation (command1::operate)          // provide the function to be executed as command
   *           .captureUndo (command1::capture)        // provide the function capturing Undo state
   *           .undoOperation (command1::undoIt)       // provide the function which might undo the command
   *           .bind (obj, randVal)                    // bind to the actual command parameters
   *           .executeSync();                         // convenience call, forwarding the Command to dispatch.
   * \endcode
   * 
   * @todo of course, this needs to be extracted into command-definition.hpp 
   */
  class CommandDef
    {
      Symbol id_;
      
      template<typename SIG, typename MEM>
      struct UndoDefinition
        {
          typedef typename FunctionSignature< function<SIG> >::Args BasicArgs;
          typedef typename FunctionTypedef<MEM,BasicArgs>::Sig      UndoCaptureSig;
          
          UndoDefinition (function<UndoCaptureSig>& undoCapOperation)
            {
              cout << showSizeof(undoCapOperation) << endl;
              UNIMPLEMENTED ("re-fetch command definition and augment it with Functor for capturing Undo information");
            }
          
          template<typename SIG2>
          UndoDefinition&
          undoOperation (SIG2& how_to_Undo)
            {
              typedef typename UndoSignature<SIG2>::UndoOp_Sig UndoSig;
              
              function<UndoSig> opera3 (how_to_Undo);
              
              UNIMPLEMENTED ("store actual Undo-Functor into the command definition held by the enclosing UndoDefinition instance");
              return *this;
            }
          
          template
            < typename T1
            , typename T2
            >
          UndoDefinition&    ///////TODO return here the completed Command
          bind ( T1& p1
               , T2& p2
               )
            {
              typedef Types<T1,T2> ArgTypes;
              Tuple<ArgTypes> params(p1,p2);
              Closure<SIG> clo (params);
              
              cout << showSizeof(clo) << endl;
              UNIMPLEMENTED ("complete Command definition by closing all functions");
              return *this;
            }
          
        };
      
      /** type re-binding helper: create a suitable UndoDefinition type,
       *  based on the UndoSignature template instance given as parameter */
      template<typename U_SIG>
      struct BuildUndoDefType
        {
          typedef UndoDefinition<typename U_SIG::OperateSig, typename U_SIG::Memento> Type;
        };
      
      template<typename SIG>
      struct BasicDefinition
        {
          BasicDefinition(function<SIG>& operation)
            {
              cout << showSizeof(operation) << endl;
              UNIMPLEMENTED ("create new command object an store the operation functor");
            }
          
          
          template<typename SIG2>
          typename BuildUndoDefType<UndoSignature<SIG2> >::Type
          captureUndo (SIG2& how_to_capture_UndoState)
            {
              typedef typename UndoSignature<SIG2>::CaptureSig UndoCapSig;
              typedef typename BuildUndoDefType<UndoSignature<SIG2> >::Type SpecificUndoDefinition;
              
              function<UndoCapSig> opera2 (how_to_capture_UndoState);
              return SpecificUndoDefinition (opera2);
            }
          
        };
      
    public:
      CommandDef (Symbol cmdID)
        : id_(cmdID)
        { }
      
      template<typename SIG>
      BasicDefinition<SIG>
      operation (SIG& operation_to_define)
        {
          function<SIG> opera1 (operation_to_define);
          return BasicDefinition<SIG>(opera1);
        }
    };
  
  ////////////////TODO currently just fleshing  out the API....
  
  
  
  
} // namespace control
#endif
