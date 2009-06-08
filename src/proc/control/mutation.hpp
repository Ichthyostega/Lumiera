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
      
    };
  
  
  /**
   * @todo Type-comment
   */
  class UndoMutation
    : public Mutation
    {
      
    public:
      
    };
  ////////////////TODO currently just fleshing  out the API....
  
  
  
  
} // namespace control
#endif
