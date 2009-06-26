/*
  COMMAND-MUTATION.hpp  -  functor encapsulating the actual operation of proc-Command
 
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


/** @file command-mutation.hpp
 ** The core of a Proc-Layer command: functor containing the actual operation to be executed.
 ** //TODO
 **  
 ** @see Command
 ** @see ProcDispatcher
 **
 */



#ifndef CONTROL_COMMAND_MUTATION_H
#define CONTROL_COMMAND_MUTATION_H

//#include "pre.hpp"
#include "lib/error.hpp"
#include "proc/control/command-closure.hpp"
#include "proc/control/memento-closure.hpp"

//#include <tr1/memory>
#include <boost/scoped_ptr.hpp>
#include <tr1/functional>
#include <iostream>
#include <string>



namespace control {
  
//  using lumiera::Symbol;
//  using std::tr1::shared_ptr;
  using boost::scoped_ptr;
  using std::tr1::function;
  using std::ostream;
  using std::string;
  
  
  LUMIERA_ERROR_DECLARE (UNBOUND_ARGUMENTS);  ///< Mutation functor not yet usable, because arguments aren't bound
  LUMIERA_ERROR_DECLARE (MISSING_MEMENTO);   ///<  Undo functor not yet usable, because no undo state has been captured
  
  
  /**
   * @todo Type-comment
   */
  class Mutation
    {
      CmdFunctor func_;
      CmdClosure* clo_;
      
    public:
      template<typename SIG>
      Mutation (function<SIG> const& func)
        : func_(func),
          clo_(0)
        { }
      
      virtual ~Mutation() {}
      
      
      virtual Mutation&
      close (CmdClosure& cmdClosure)
        {
          REQUIRE (!clo_, "Lifecycle error: already closed over the arguments");
          REQUIRE (func_, "Param error: not bound to a valid function");
          func_ = cmdClosure.bindArguments(func_);
          clo_ = &cmdClosure;
          return *this;
        }
      
      
      void
      operator() ()
        {
          if (!clo_)
            throw lumiera::error::State ("Lifecycle error: function arguments not yet provided",
                                         LUMIERA_ERROR_UNBOUND_ARGUMENTS);
          invoke (func_);
        }
      
      
      /* == diagnostics == */
      typedef CmdClosure* Mutation::*_unspecified_bool_type;
      
      /** implicit conversion to "bool" */ 
      operator _unspecified_bool_type()  const { return  isValid()? &Mutation::clo_ : 0; }  // never throws
      bool operator! ()                  const { return !isValid(); }                      //  ditto
      
      operator string()  const
        { 
          return isValid()? string (*clo_) : "Mutation(state missing)";
        }
      
    protected:
      virtual bool
      isValid ()   const
        {
          return func_ && clo_;
        }
      
      void
      invoke (CmdFunctor & closedFunction)
        {
          closedFunction.getFun<void()>() ();
        }
      
      CmdClosure&
      getClosure() ///< Interface for subclasses to access the bound parameters
        {
          REQUIRE (clo_, "Lifecycle error: function arguments not yet bound");
          return *clo_;
        }
    };
  
  
  inline ostream& operator<< (ostream& os, const Mutation& muta) { return os << string(muta); }
  
  
  
  
  /**
   * @todo Type-comment
   */
  class UndoMutation
    : public Mutation
    {
      CmdFunctor         captureFunc_;
      scoped_ptr<CmdClosure> memento_;
      
    public:
      template<typename SIG_undo, typename SIG_cap>
      UndoMutation (function<SIG_undo> const& undoFunc,
                    function<SIG_cap> const& captureFunc)
        : Mutation (undoFunc)
        , captureFunc_(captureFunc)
        { }
      
      UndoMutation (UndoMutation const& o)
        : Mutation (*this)
        , captureFunc_(o.captureFunc_)
        , memento_(o.memento_->clone().get())
        { }
      
      UndoMutation&
      operator= (UndoMutation const& o)
        {
          Mutation::operator= (o);
          captureFunc_ = o.captureFunc_;
          memento_.reset(o.memento_->clone().get());
          return *this;
        }
      
      
      virtual Mutation&
      close (CmdClosure& cmdClosure)
        {
          REQUIRE (!memento_,    "Lifecycle error: already closed over the arguments");
          REQUIRE (captureFunc_, "Param error: not bound to a valid function");
          
          // create a special state closure, which can later on store the captured undo state (memento)
          scoped_ptr<CmdClosure> stateClosure (new MementoClosure (captureFunc_));
          CmdFunctor closedCaptureFunc = stateClosure->bindArguments(captureFunc_);
          
          // the undoFunc (within parent class) will retrieve an argument tuple extended by the memento
//          Mutation::close (stateClosure->decorate (cmdClosure));
          
          captureFunc_ = closedCaptureFunc;
          memento_.swap(stateClosure);
          return *this;
        }
      
      Mutation&
      captureState ()
        {
          if (!Mutation::isValid())
            throw lumiera::error::State ("need to bind function arguments prior to capturing undo state",
                                         LUMIERA_ERROR_UNBOUND_ARGUMENTS);
          
          invoke(captureFunc_);
          return *this;
        }
      
      CmdClosure&
      getMemento()
        {
          ASSERT (memento_, "Lifecycle error: need to close first");
          return *memento_;
        }
      
    private:
      virtual bool
      isValid ()   const
        {
          return Mutation::isValid() && captureFunc_ && memento_;
        }
      
      
    };
  ////////////////TODO currently just fleshing  out the API....
  
    
  
  
} // namespace control
#endif
