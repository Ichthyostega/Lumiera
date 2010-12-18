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
 ** \par Relation of function signatures
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

//#include "pre.hpp"
//#include "lib/symbol.hpp"
#include "lib/meta/function.hpp"
#include "lib/meta/typelist.hpp"
#include "lib/meta/typelist-util.hpp"
#include "lib/meta/typeseq-util.hpp"
//#include "lib/meta/tuple.hpp"

//#include <tr1/memory>
#include <tr1/functional>




namespace control {
  
//  using lib::Symbol;
//  using std::tr1::shared_ptr;
  using std::tr1::function;
  
  using lumiera::typelist::FunctionSignature;
  using lumiera::typelist::FunctionTypedef;
  using lumiera::typelist::Types;
//using lumiera::typelist::NullType;
//using lumiera::typelist::Tuple;
  using lumiera::typelist::Append;
  using lumiera::typelist::SplitLast;
  

  /** 
   * Metaprogramming helper for building Command function signatures. 
   * The complete definition context of any command is templated to the signature
   * of the actual command operation and to the memento type. The typedefs embedded
   * within CommandSignature<SIG,MEM> allows accepting suitable typed functions
   * to implement the command in question.
   */
  template<typename SIG, typename MEM>
  class CommandSignature
    {
      typedef typename FunctionSignature< function<SIG> >::Args Args;
      
      typedef typename Args::List ArgList;
      typedef typename Append<ArgList, MEM>::List ExtendedArglist;
      typedef typename Types<ExtendedArglist>::Seq ExtendedArgs;
      
    public:
      typedef typename FunctionTypedef<void, Args>::Sig          OperateSig;
      typedef typename FunctionTypedef<MEM, Args>::Sig           CaptureSig;
      typedef typename FunctionTypedef<void, ExtendedArgs>::Sig  UndoOp_Sig;
      typedef Args                                               CmdArgs;
      typedef MEM                                                Memento;
    };
  
  
  
  
  /** 
   * Type analysis helper template. 
   * Used for dissecting a given type signature to derive
   * the related basic operation signature, the signature of a possible Undo-function
   * and the signature necessary for capturing undo information. The implementation
   * relies on re-binding an embedded type defining template, based on the actual
   * case, as identified by the structure of the given parameter signature.
   */
  template<typename SIG>
  class UndoSignature
    {
      typedef typename FunctionSignature< function<SIG> >::Args Args;
      typedef typename FunctionSignature< function<SIG> >::Ret  Ret;
      
      /** Case1: defining the Undo-Capture function */
      template<typename RET, typename ARG>
      struct Case
        {
          typedef RET Memento;
          typedef typename Append<ARG, Memento>::List ExtendedArglist;
          typedef typename Types<ExtendedArglist>::Seq ExtendedArgs;
          
          typedef typename FunctionTypedef<void, ARG>::Sig           OperateSig;
          typedef typename FunctionTypedef<Ret,ARG>::Sig             CaptureSig;
          typedef typename FunctionTypedef<void, ExtendedArgs>::Sig  UndoOp_Sig;
        };
      /** Case2: defining the actual Undo function */
      template<typename ARG>
      struct Case<void,ARG>
        {
          typedef typename ARG::List Args;
          
          typedef typename SplitLast<Args>::Type Memento;
          typedef typename SplitLast<Args>::List OperationArglist;
          typedef typename Types<OperationArglist>::Seq OperationArgs;
          
          typedef typename FunctionTypedef<void, OperationArgs>::Sig OperateSig;
          typedef typename FunctionTypedef<Ret,OperationArgs>::Sig   CaptureSig;
          typedef typename FunctionTypedef<void, ARG>::Sig           UndoOp_Sig;
        };
      
    public:
      typedef typename Case<Ret,Args>::CaptureSig CaptureSig;
      typedef typename Case<Ret,Args>::UndoOp_Sig UndoOp_Sig;
      typedef typename Case<Ret,Args>::OperateSig OperateSig;
      typedef typename Case<Ret,Args>::Memento    Memento;
    };
  
  
  
} // namespace control
#endif
