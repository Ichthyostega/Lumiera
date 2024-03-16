/*
  COMMAND-SIGNATURE.hpp  -  deriving suitable command function signatures

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


/** @file command-signature.hpp
 ** Metaprogramming helpers for deriving the precise function signatures
 ** necessary to implement a given command. Basically, commands can be implemented
 ** by arbitrary functions, but the signatures of the operation function, the
 ** undo function and the undo state capturing function are required to obey
 ** fixed relationships. Thus, at various stages of the command definition,
 ** we need to accept functor objects with a very specific and predetermined
 ** signature, thus allowing for strict type checking by the compiler.
 ** 
 ** \par Relation of function signatures (MEM = type of the "memento" for Undo)
 ** - operation: void(P1,..PN)
 ** - captureUndo: MEM(P1,..PN)
 ** - undoOperation void(P1,..PN,MEM)
 ** - bind takes the arguments:(P1,..PN)
 ** 
 ** @see Command
 ** @see CommandDef
 **
 */



#ifndef CONTROL_COMMAND_SIGNATURE_H
#define CONTROL_COMMAND_SIGNATURE_H

#include "lib/meta/function.hpp"
#include "lib/meta/typelist.hpp"
#include "lib/meta/typelist-manip.hpp"
#include "lib/meta/typeseq-util.hpp"

#include <functional>



namespace lumiera {
namespace error {
  LUMIERA_ERROR_DECLARE (UNBOUND_ARGUMENTS); ///< Command functor not yet usable, because arguments aren't bound
  LUMIERA_ERROR_DECLARE (MISSING_MEMENTO);  ///<  Undo functor not yet usable, because no undo state has been captured
}}

namespace steam {
namespace control {
  
  using std::function;
  
  using lib::meta::BuildFunType;
  using lib::meta::_Fun;
  using lib::meta::Types;
  using lib::meta::Append;
  using lib::meta::SplitLast;
  
  
  /** 
   * Metaprogramming helper for building Command function signatures.
   * The complete definition context of any command is templated to the signature
   * of the actual command operation and to the memento type. The typedefs embedded
   * within CommandSignature<SIG,MEM> allows for accepting suitable typed functions
   * to implement the command in question.
   */
  template<typename SIG, typename MEM>
  class CommandSignature
    {
      using Args            = typename _Fun<SIG>::Args;
      using ArgList         = typename Args::List;
      using ExtendedArglist = typename Append<ArgList, MEM>::List;
      using ExtendedArgs    = typename Types<ExtendedArglist>::Seq;
      
    public:
      using OperateSig = typename BuildFunType<void, Args>::Sig;
      using CaptureSig = typename BuildFunType<MEM,  Args>::Sig;
      using UndoOp_Sig = typename BuildFunType<void, ExtendedArgs>::Sig;
      using CmdArgs    = Args;
      using Memento    = MEM;
    };
  
  
  
  
  /** 
   * Type analysis helper template.
   * Used for dissecting a given type signature to derive
   * the related basic operation signature, the signature of a possible Undo-function
   * and the signature necessary for capturing undo information. The implementation
   * relies on re-binding an embedded type defining template, based on the actual
   * case, as identified by the structure of the given parameter signature.
   * 
   * To use this template, it is instantiated with the signature of a functor object
   * in question. Depending on the actual situation, the compiler will then either
   * pick Case1 or Case2 -- thus allowing the client in any case to pick up the
   * correct signatures for Operation, Capture and Undo-function from the
   * public typedefs within \c UndoSignature
   */
  template<typename SIG>
  class UndoSignature
    {
      // preparation:  dissect the function signature into arguments and result
      using Args = typename _Fun<SIG>::Args;
      using Ret  = typename _Fun<SIG>::Ret;
      
      /** Case1: defining the Undo-Capture function */
      template<typename RET, typename ARG>
      struct Case
        {
          using Memento = RET;
          
          using ExtendedArglist = typename Append<ARG, Memento>::List;
          using ExtendedArgs    = typename Types<ExtendedArglist>::Seq;
          
          using OperateSig = typename BuildFunType<void, ARG>::Sig;
          using CaptureSig = typename BuildFunType<Ret,ARG>::Sig;
          using UndoOp_Sig = typename BuildFunType<void, ExtendedArgs>::Sig;
        };
      /** Case2: defining the actual Undo function */
      template<typename ARG>
      struct Case<void,ARG>
        {
          using Args = typename ARG::List;
          
          using Memento          = typename SplitLast<Args>::Type;
          using OperationArglist = typename SplitLast<Args>::List;
          using OperationArgs    = typename Types<OperationArglist>::Seq;
          
          using OperateSig = typename BuildFunType<void, OperationArgs>::Sig;
          using CaptureSig = typename BuildFunType<Ret,OperationArgs>::Sig;
          using UndoOp_Sig = typename BuildFunType<void, ARG>::Sig;
        };
      
    public:
      using CaptureSig = typename Case<Ret,Args>::CaptureSig;
      using UndoOp_Sig = typename Case<Ret,Args>::UndoOp_Sig;
      using OperateSig = typename Case<Ret,Args>::OperateSig;
      using Memento    = typename Case<Ret,Args>::Memento;
    };
  
  
  
}} // namespace steam::control
#endif
