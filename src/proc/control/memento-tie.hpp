/*
  MEMENTO-TIE.hpp  -  capturing and providing state for undoing commands
 
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


/** @file memento-tie.hpp
 ** A special binding used by Proc-Layer commands for capturing UNDO state information.
 ** The UndoMutation, which is the functor object created and configured by the Command
 ** for handling UNDO, utilises a MementoTie (as ctor parameter) for binding together
 ** the "undo capture function" and the actual "undo function", by retrieving the
 ** memento data or memento object from the former and feeding it to the latter
 ** as an additional parameter, when the undo operation is invoked.
 **  
 ** @see CmdClosure
 ** @see UndoMutation
 ** @see memento-tie-test.cpp
 **
 */



#ifndef CONTROL_MEMENTO_TIE_H
#define CONTROL_MEMENTO_TIE_H

//#include "pre.hpp"
//#include "lib/meta/typelist.hpp"  ////////////////TODO include these??
//#include "lib/meta/function.hpp"
#include "lib/meta/function-closure.hpp"
//#include "lib/meta/function-erasure.hpp"
//#include "lib/meta/tuple.hpp"
#include "proc/control/command-signature.hpp"
#include "proc/control/command-closure.hpp" ////////TODO really??
#include "lib/util.hpp"

//#include <tr1/memory>
#include <tr1/functional>
//#include <iostream> 
//#include <string>


#include "lib/test/test-helper.hpp"  /////////////////TODO remove this
using lib::test::showSizeof;

using std::cout;    //////////////////////////////////TODO remove this
using std::endl;


namespace control {
  
//  using lumiera::Symbol;
//  using std::tr1::shared_ptr;
//  using util::unConst;
  using std::string;
  using lumiera::typelist::func::bindLast;
  using lumiera::typelist::func::chained;
//  using std::ostream;
//  using std::tr1::function;
//  using lumiera::typelist::FunctionSignature;
//  using lumiera::typelist::Tuple;
//  using lumiera::typelist::BuildTupleAccessor;
//  using lumiera::typelist::TupleApplicator;
//  using lumiera::typelist::FunErasure;
//  using lumiera::typelist::StoreFunction;
  
//  using lumiera::typelist::NullType;
    
  LUMIERA_ERROR_DECLARE (MISSING_MEMENTO);  ///<  Undo functor not yet usable, because no undo state has been captured
  
  
  /**
   *  Binding together state capturing and execution of the undo operation.
   *  MementoTie itself is a passive container object with a very specific type,
   *  depending on the type of the operation arguments and the type of the memento.
   *  It is to be allocated within the ArgumentHolder of the command, thereby wrapping
   *  or decorating the undo and capture function, setting up the necessary bindings and
   *  closures, allowing them to cooperate behind the scenes to carry out the UNDO functionality.
   *  Through a reference to the MementoTie, the UndoMutation functor gets access to the prepared
   *  functions, storing them into generic containers (type erasure) for later invocation. 
   *  
   *  More specifically, the \c captureFunction, which is expected to run immediately prior
   *  to the actual command operation, returns a \b memento value object (of unspecific type),
   *  which needs to be stored within the MementoTie. On UNDO, the undo-operation functor needs
   *  to be provided with a reference to this stored memento value through an additional
   *  parameter (which by convention is always the last argument of the undo function).
   *  
   */
  template<typename SIG, typename MEM>
  class MementoTie
    {
      typedef typename CommandSignature<SIG,MEM>::CaptureSig SIG_cap;
      typedef typename CommandSignature<SIG,MEM>::UndoOp_Sig SIG_undo;
      
      /** storage holding the captured state for undo */
      MEM memento_;
      
      bool isCaptured_;
      
      function<SIG_undo> undo_;
      function<SIG_cap> capture_;
      
      void capture (MEM const& mementoVal)
        {
          memento_ = mementoVal;
          isCaptured_ = true;
        }
      
    public:
      /** creates an execution context tying together the provided functions.
       *  Bound copies of these functors may be pulled from the MementoTie,
       *  in order to build the closures (with the concrete operation arguments)
       *  to be invoked later on command execution.
       */
      MementoTie (function<SIG_undo> const& undoFunc,
                  function<SIG_cap> const& captureFunc)
        : memento_()
        , isCaptured_(false)
        , undo_(undoFunc)
        , capture_(captureFunc)
        { }
      
      
      /** bind the undo function to the internal memento store within this object.
       *  @return a functor, which, when invoked with the remaining arguments, will
       *        automatically call #getState() to retrieve the current memento value.
       *  @note similar to #getState(), the returned functor will throw
       *        when the state capturing wasn't yet invoked
       */
      function<SIG>
      tieUndoFunc()
        {
          using std::tr1::bind;
          
          return bindLast( undo_
                         , bind (&MementoTie::getState, this)
                         );
        }
      
      /** bind the capturing function to the internal memento store within this object.
       *  @return a functor, which on invocation will automatically store the return value
       *        of the capturing function (= the current memento value) into the field
       *        #memento_ within this object 
       */
      function<SIG>
      tieCaptureFunc()
        {
          using std::tr1::placeholders::_1;
          
          function<void(MEM)> doCaptureMemento = bind (&MementoTie::capture, this, _1);
          
          return chained(capture_, doCaptureMemento);
        }
      
      
      /** get the currently captured memento state value
       *  @throw when the capturing function wasn't yet invoked
       */
      MEM&
      getState ()
        {
          if (!isCaptured_)
            throw lumiera::error::State ("need to invoke memento state capturing beforehand",
                                         LUMIERA_ERROR_MISSING_MEMENTO);
          return memento_;
        }
      
    };
  
  
  
  
} // namespace control
#endif
