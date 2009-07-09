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

#include "lib/bool-checkable.hpp"
#include "lib/meta/function-closure.hpp"
#include "proc/control/command-signature.hpp"
#include "lib/util.hpp"

#include <tr1/functional>


namespace control {
  
  using std::tr1::ref;
  using lumiera::typelist::func::bindLast;
  using lumiera::typelist::func::chained;
    
  LUMIERA_ERROR_DECLARE (MISSING_MEMENTO);  ///<  Undo functor not yet usable, because no undo state has been captured
  
  
  /**
   *  Binding together state capturing and execution of the undo operation.
   *  MementoTie itself is a passive container object with a very specific type,
   *  depending on the type of the operation arguments and the type of the memento.
   *  It is to be allocated within the ArgumentHolder of the command, thereby wrapping
   *  the undo and capture function, setting up the necessary bindings and closures for
   *  allowing them to cooperate behind the scenes to carry out the UNDO functionality.
   *  On construction, the UndoMutation functor retrieves the wired up functions,
   *  storing them into generic containers (type erasure) for later invocation. 
   *  
   *  More specifically, the \c captureFunction, which is expected to run immediately prior
   *  to the actual command operation, returns a \b memento value object (of unspecific type),
   *  which needs to be stored within the MementoTie. On UNDO, the undo-operation functor needs
   *  to be provided with a reference to this stored memento value through an additional
   *  parameter (which by convention is always the last argument of the undo function).
   */
  template<typename SIG, typename MEM>
  class MementoTie
    : public lib::BoolCheckable<MementoTie<SIG,MEM> >
    {
      typedef typename CommandSignature<SIG,MEM>::CaptureSig SIG_cap;
      typedef typename CommandSignature<SIG,MEM>::UndoOp_Sig SIG_undo;
      
      MEM memento_; ///< storage holding the captured state for undo
      
      bool isCaptured_;
      
      function<SIG_undo> undo_;
      function<SIG_cap> capture_;
      
      
      /* == internal functionality binding the functions to the memento == */
      
      /** to be chained behind the capture function */
      void capture (MEM const& mementoVal)
        {
          memento_ = mementoVal;
          isCaptured_ = true;
        }
      
      /** partially closed undo function:
       *  on invocation, receive the current memento value
       *  and feed it to the wrapped undo function. Additionally
       *  provides a \c bool() conversion to check if any memento
       *  state has yet been captured.  
       */
      struct WiredUndoFunc
        : function<SIG> 
        , lib::BoolCheckable<WiredUndoFunc>
        {
          
          WiredUndoFunc (MementoTie& thisTie)
            : function<SIG> (
                bindLast( thisTie.undo_   // getState() bound to last argument of undo(...)
                        , bind (&MementoTie::getState, ref(thisTie))
                        )   )
            , mementoHolder_(thisTie)
            { }
          
          bool
          isValid () const
            {
              return function<SIG>::operator bool()
                  && bool(mementoHolder_);
            }
          
        private:
          MementoTie & mementoHolder_;
        };
      
      
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
      WiredUndoFunc
      tieUndoFunc()
        {
          return WiredUndoFunc(*this);
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
          
          function<void(MEM)> doCaptureMemento = bind (&MementoTie::capture, this, _1 );
          
          return chained(capture_, doCaptureMemento);
        }
      
      
      /** access the currently captured memento state value
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
      
      
      /** conversion to bool() yields true
       *  if all functions are usable and 
       *  memento state has been captured
       */
      bool
      isValid ()  const
        {
          return undo_ && capture_ && isCaptured_;
        }
    };
  
  
  
  
} // namespace control
#endif
