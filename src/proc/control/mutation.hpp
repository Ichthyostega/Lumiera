/*
  MUTATION.hpp  -  a functor encapsulating the actual operation of a proc-Command
 
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


/** @file mutation.hpp
 ** The core of a proc-Layer command: functor containing the actual operation to be executed.
 ** //TODO
 **  
 ** @see Command
 ** @see ProcDispatcher
 **
 */



#ifndef CONTROL_MUTATION_H
#define CONTROL_MUTATION_H

//#include "pre.hpp"
#include "lib/error.hpp"

//#include <tr1/memory>



namespace control {
  
//  using lumiera::Symbol;
//  using std::tr1::shared_ptr;
  
  
  /**
   * @todo Type-comment
   */
  class Mutation
    {
      
    public:
      template<typename SIG>
      Mutation (function<SIG> const& func)
        {
          UNIMPLEMENTED ("build a Mutation functor object, thereby erasing the concrete function signature type");
        }
      
      virtual ~Mutation() {}
      
      virtual Mutation&
      close (CmdClosure const& closure)
        {
          UNIMPLEMENTED ("accept and store a parameter closure");
        }
      
      void
      operator() ()
        {
          UNIMPLEMENTED ("invoke the Mutation functor");
        }
      
      
      /* == diagnostics == */
      typedef pClo Mutation::*_unspecified_bool_type;
      
      /** implicit conversion to "bool" */ 
      operator _unspecified_bool_type()  const { return  isValid()? &Mutation::clo_ : 0; }  // never throws
      bool operator! ()                  const { return !isValid(); }                      //  ditto
      
      operator string()  const
        { 
          return isValid()? string (*clo) : "Mutation(state missing)";
        }
      
    protected:
      virtual bool
      isValid () 
        {
          UNIMPLEMENTED ("mutation lifecycle");
        }
    };
  
  
  inline ostream& operator<< (ostream& os, const Mutation& muta) { return os << string(muta); }
  
  
  
  
  /**
   * @todo Type-comment
   */
  class UndoMutation
    : public Mutation
    {
      
    public:
      template<typename SIG>
      UndoMutation (function<SIG> const& func)
        {
          UNIMPLEMENTED ("build a undo Mutation functor object");
        }
      
      Mutation&
      captureState ()
        {
          UNIMPLEMENTED ("invoke the state capturing Functor");
        }
      
      CmdClosure&
      getMemento()
        {
          UNIMPLEMENTED ("return the closure serving as memento");
        }
      
    };
  ////////////////TODO currently just fleshing  out the API....
  
  
  
  
} // namespace control
#endif
