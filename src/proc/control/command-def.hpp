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
 ** Besides, you provide a \em binding, thus creating a closure out of these three
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
#include "lib/error.hpp"
#include "include/logging.h"
#include "lib/symbol.hpp"
#include "proc/control/command.hpp"
#include "proc/control/command-impl.hpp"                   /////TODO: any chance to get rid of this import here??
#include "proc/control/command-registry.hpp"
#include "proc/control/command-signature.hpp"
#include "proc/control/command-mutation.hpp"
#include "proc/control/command-closure.hpp"
#include "proc/control/argument-tuple-accept.hpp"
#include "lib/bool-checkable.hpp"
#include "lib/meta/function.hpp"
#include "lib/meta/typelist.hpp"
#include "lib/meta/typelist-util.hpp"
#include "lib/meta/tuple.hpp"
#include "lib/util.hpp"

#include <tr1/memory>
#include <tr1/functional>




namespace control {
  
  using std::tr1::shared_ptr;
  using std::tr1::function;
  using lib::Symbol;
  using util::cStr;
  
  using lumiera::typelist::FunctionSignature;
  using lumiera::typelist::FunctionTypedef;
  using lumiera::typelist::Types;
  using lumiera::typelist::NullType;
  using lumiera::typelist::Tuple;
  
  
  namespace stage { ///< helpers for building up a command definition
    
    
    
    template<typename SIG>
    struct CompletedDefinition
      : AcceptArgumentBindingRet< Command&, SIG               // Return type and Argument Signature of the \c bind(..) function
                                , CompletedDefinition<SIG>   //  target type (this class) providing the implementation \c bindArg(Tuple<..>) 
                                >
      {
        Command& prototype_;
        
        typedef typename FunctionSignature< function<SIG> >::Args CmdArgs;
        
        CompletedDefinition (Command& definedCommand)
          : prototype_(definedCommand)
          {
            TRACE (command_dbg, "Completed definition of %s.", cStr(prototype_));
          }
        
        typedef HandlingPattern::ID PattID;
        
        PattID getDefaultHandlingPattern()  const { return prototype_.getDefaultHandlingPattern();}
        PattID setHandlingPattern (PattID newID)  { return prototype_.setHandlingPattern(newID); }
        
        Command&
        bindArg (Tuple<CmdArgs> const& params)
          {
            return prototype_.bindArg(params);
          }
      };
    
    
    
    
    
    template<typename SIG, typename MEM>
    struct UndoDefinition
      {
        typedef CommandSignature<SIG,MEM> CmdType;
        typedef typename CmdType::OperateSig CommandOperationSig;
        typedef typename CmdType::UndoOp_Sig UndoOperationSig;
        typedef typename CmdType::CaptureSig UndoCaptureSig;
        typedef typename CmdType::CmdArgs    CmdArgs;
        
        typedef function<CommandOperationSig> OperFunc;
        typedef function<UndoOperationSig>    UndoFunc;
        typedef function<UndoCaptureSig>      CaptFunc;
        
        Command& prototype_;
        OperFunc operFunctor_;
        CaptFunc captFunctor_;
        UndoFunc undoFunctor_;
        
        
        UndoDefinition (Command& underConstruction, 
                        OperFunc& commandOperation,
                        CaptFunc& undoCapOperation)
          : prototype_(underConstruction)
          , operFunctor_(commandOperation)
          , captFunctor_(undoCapOperation)
          , undoFunctor_()
          { }
        
        
        CompletedDefinition<SIG>
        undoOperation (UndoOperationSig& how_to_Undo)
          {
            undoFunctor_ = UndoFunc (how_to_Undo);
            REQUIRE (operFunctor_);
            REQUIRE (undoFunctor_);
            REQUIRE (captFunctor_);
            
            typedef shared_ptr<CommandImpl> ImplInstance;
            CommandRegistry& registry = CommandRegistry::instance();
            ImplInstance completedDef = registry.newCommandImpl(operFunctor_
                                                               ,captFunctor_
                                                               ,undoFunctor_);
            prototype_.activate(completedDef);
            // activated the instance in the registry 
            ENSURE (prototype_);
            return CompletedDefinition<SIG> (prototype_);
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
        Command& prototype_;
        function<SIG>& operation_;
        
        BasicDefinition(Command& underConstruction, function<SIG>& operation)
          : prototype_(underConstruction)
          , operation_(operation)
          { }
        
        
        template<typename SIG2>
        typename BuildUndoDefType<UndoSignature<SIG2> >::Type
        captureUndo (SIG2& how_to_capture_UndoState)
          {
            typedef typename UndoSignature<SIG2>::CaptureSig UndoCapSig;
            typedef typename BuildUndoDefType<UndoSignature<SIG2> >::Type SpecificUndoDefinition;
            
            function<UndoCapSig> captureOperation (how_to_capture_UndoState);
            return SpecificUndoDefinition (prototype_, operation_, captureOperation);
          }
      };
    
  } // (END) namespace stage (definition process)
  
  
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
   */
  class CommandDef
    : public lib::BoolCheckable<CommandDef>
    {
      Symbol id_;
      Command& prototype_;
      
    public:
      CommandDef (Symbol cmdID)
        : id_(cmdID)
        , prototype_(Command::fetchDef(cmdID))
        {
          TRACE (command_dbg, "starting CommandDef('%s')...", cmdID.c() );
        }
      
      template<typename SIG>
      stage::BasicDefinition<SIG>
      operation (SIG& operation_to_define)
        {
          function<SIG> opera1 (operation_to_define);
          return stage::BasicDefinition<SIG>(prototype_, opera1);
        }
      
      
      bool isValid()  const;
    };
  
  
  
  
  
} // namespace control
#endif
