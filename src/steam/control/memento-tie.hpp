/*
  MEMENTO-TIE.hpp  -  capturing and providing state for undoing commands

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


/** @file memento-tie.hpp
 ** A special binding used by Steam-Layer commands for capturing UNDO state information.
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

#include "lib/meta/maybe-compare.hpp"
#include "lib/meta/function-closure.hpp"
#include "steam/control/command-signature.hpp"
#include "lib/replaceable-item.hpp"
#include "lib/format-obj.hpp"
#include "lib/util.hpp"

#include <boost/operators.hpp>
#include <functional>
#include <string>


namespace steam {
namespace control {
  namespace err = lumiera::error;
  
  using boost::equality_comparable;
  using lib::meta::func::bindLast;
  using lib::meta::func::chained;
  using lib::meta::equals_safeInvoke;
  using lib::wrapper::ReplaceableItem;
  
  
  /**
   *  Binding together state capturing and execution of the undo operation.
   *  MementoTie itself is a passive container object with a very specific type,
   *  depending on the type of the operation arguments and the type of the memento.
   *  It is to be allocated within the StorageHolder of the command, thereby wrapping
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
   *  @warning take care of the MementoTie storage location, as the bound functions
   *           returned by #tieCaptureFunc and #tieUndoFunc refer to `this` implicitly.
   *  
   *  @param SIG signature of the command operation
   *  @param MEM type of the memento state to capture. Needs to be default constructible and copyable
   */
  template<typename SIG, typename MEM>
  class MementoTie
    : public equality_comparable<MementoTie<SIG,MEM>>
    {
      typedef typename CommandSignature<SIG,MEM>::CaptureSig SIG_cap;
      typedef typename CommandSignature<SIG,MEM>::UndoOp_Sig SIG_undo;
      
      ReplaceableItem<MEM> memento_; ///< storage holding the captured state for undo
      
      bool isCaptured_;
      
      function<SIG_undo> undo_;
      function<SIG_cap> capture_;
      
      
      /** to be chained behind the capture function */
      void capture (MEM const& mementoVal)
        {
          memento_ = mementoVal;
          isCaptured_ = true;
        }
      
      
    public:
      MementoTie()
        : MementoTie (function<SIG_undo>(), function<SIG_cap>())
        { }
      
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
      
      
      /** reverses the effect of capturing state
       *  and returns this memento holder into pristine state
       */
      void
      clear()
        {
          isCaptured_ = false;
          memento_.clear();
        }
      
      /** bind the undo function to the internal memento store within this object.
       *  @return a functor, which, when invoked with the remaining arguments, will
       *        automatically call #getState() to retrieve the current memento value.
       *  @note similar to #getState(), the returned functor will throw
       *        when the state capturing wasn't yet invoked
       */
      function<SIG>
      tieUndoFunc()
        {
          using std::bind;
          
          return bindLast( undo_           // getState() bound to last argument of undo(...)
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
          return chained(capture_
                        ,[this](MEM const& mementoVal){ capture (mementoVal); }
                        );
        }
      
      
      /** access the currently captured memento state value
       *  @throw when the capturing function wasn't yet invoked
       */
      MEM&
      getState ()
        {
          if (!isCaptured_)
            throw err::State{"need to invoke memento state capturing beforehand"
                            , LERR_(MISSING_MEMENTO)};
          return memento_;
        }
      
      
      /** conversion to bool() yields true
       *  if all functions are usable and
       *  memento state has been captured
       */
      explicit
      operator bool()  const
        {
          return isValid();
        }
      
      bool
      isValid ()  const
        {
          return undo_ && capture_ && isCaptured_;
        }
      
      /** for diagnostics: include format-util.hpp */
      operator std::string()  const;
      
    };
  
  
  template<typename SIG, typename MEM>
  MementoTie<SIG,MEM>::operator std::string()  const
  {
    if (!undo_ || !capture_)
      return "·noUNDO·";
    
    if (!isCaptured_)
      return "<mem:missing>";
    
    return "<mem: "
         + util::toString (memento_.get())
         + ">";
  }
  
  
  
}} // namespace steam::control
#endif
