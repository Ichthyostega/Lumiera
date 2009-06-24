/*
  MEMENTO-CLOSURE.hpp  -  capturing and providing state for undoing commands
 
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


/** @file memento-closure.hpp
 ** Extension to the CmdClosure for storing and retrieving a state memento.
 ** //TODO
 **  
 ** @see CmdClosure
 ** @see UndoMutation
 ** @see memento-closure-test.cpp
 **
 */



#ifndef CONTROL_MEMENTO_CLOSURE_H
#define CONTROL_MEMENTO_CLOSURE_H

//#include "pre.hpp"
//#include "lib/meta/typelist.hpp"  ////////////////TODO include these??
//#include "lib/meta/function.hpp"
//#include "lib/meta/function-closure.hpp"
//#include "lib/meta/function-erasure.hpp"
//#include "lib/meta/tuple.hpp"
#include "proc/control/command-closure.hpp"
#include "lib/util.hpp"

//#include <tr1/memory>
//#include <tr1/functional>
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
//  using std::ostream;
//  using std::tr1::function;
//  using lumiera::typelist::FunctionSignature;
//  using lumiera::typelist::Tuple;
//  using lumiera::typelist::BuildTupleAccessor;
//  using lumiera::typelist::TupleApplicator;
//  using lumiera::typelist::FunErasure;
//  using lumiera::typelist::StoreFunction;
  
//  using lumiera::typelist::NullType;
    
  
  
  /**
   *  Special kind of Closure, which \em decorates an existing Closure
   *  and provides a captured state memento as additional parameter on invocation.
   *  
   *  @todo concept isn't clear yet. Multiple MementoClosurese are to decorate a single Closure;
   *        they have to match and extract the concrete type of the Closure and the provided Memento,
   *        but the latter needs to be erased immediately. Basically, MementoClosure must be able
   *        to stand-in for an simple parameter closure. 
   */
  class MementoClosure
    : public CmdClosure   ///////////TODO hierarachy?
    {
    public:
      
      virtual PClosure clone()  const =0;
      
      virtual operator string()  const =0;
      
      virtual CmdFunctor bindArguments (CmdFunctor&) =0;
      
      
      ////////////TODO how to give access to the following dedicated API?
      CmdClosure& decorate (CmdClosure& core) 
        {
          return *this; // TODO would be nice, but probably won't be thus simple ;-) 
        }
    };
    
  ////////////////TODO currently just fleshing  out the API....
  
  
  
  
} // namespace control
#endif
