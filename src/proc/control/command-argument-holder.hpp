/*
  COMMAND-ARGUMENT-HOLDER.hpp  -  specifically typed container for storage of command arguments
 
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


/** @file command-argument-holder.hpp
 ** A simple container record holding the actual command arguments. 
 ** While all command objects themselves have a common type (type erasure),
 ** the actual argument tuple and the state memento for undo can't. Especially,
 ** the size of arguments and memento will depend on their respective types.
 ** Thus, to manage somehow the storage of this data, we create a common holder,
 ** to be managed by a custom allocator.
 ** 
 ** @todo doing just plain heap allocation for now :-P
 **  
 ** @see Command
 ** @see UndoMutation
 ** @see MementoTie
 ** @see command-argument-test.cpp
 **
 */



#ifndef CONTROL_COMMAND_ARGUMENT_HOLDER_H
#define CONTROL_COMMAND_ARGUMENT_HOLDER_H

//#include "pre.hpp"
//#include "lib/error.hpp"

//#include <tr1/memory>
//#include <boost/scoped_ptr.hpp>
//#include <tr1/functional>
#include <iostream>
#include <string>



namespace control {
  
//  using lumiera::Symbol;
//  using std::tr1::shared_ptr;
//  using boost::scoped_ptr;
//  using std::tr1::function;
  using std::ostream;
  using std::string;
  
  
  
  /**
   * @todo Type-comment
   */
  class CommandArgumentHolder
    {
      
    public:
      virtual ~CommandArgumentHolder() { };
      virtual operator string()  const  =0;
    };
  
  
  inline ostream& operator<< (ostream& os, CommandArgumentHolder const& arg) { return os << string(arg); }
  
  
  
  
  /** specifically typed subclass used for actual storage */
  template<typename SIG, typename MEM>
  class ArgumentHolder
    : public CommandArgumentHolder
    {
      
      operator string()  const { return "bääääh!"; } /////////////////TODO
      
    };
  ////////////////TODO currently just fleshing  out the API....
  
    
  
  
} // namespace control
#endif
