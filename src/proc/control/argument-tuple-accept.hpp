/*
  ARGUMENT-TUPLE-ACCEPT.hpp  -  helper template providing a bind(...) member function
 
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


/** @file argument-tuple-accept.hpp
 ** This template allows to mix in a \c bind(...) function.
 ** Thereby, the correct number and types of arguments is derived
 ** according to the function signature given as template parameter.
 ** This helper template is used for the ArgumentHolder and generally
 ** for binding arguments for Proc-Layer commands.
 ** 
 ** @see CommandDef
 ** @see ArgumentHolder
 ** @see argument-tuple-accept-test.cpp
 **
 */



#ifndef CONTROL_ARGUMENT_TUPLE_ACCEPT_H
#define CONTROL_ARGUMENT_TUPLE_ACCEPT_H

//#include "pre.hpp"
//#include "lib/error.hpp"
#include "lib/meta/typelist.hpp"
#include "lib/meta/tuple.hpp"

//#include <tr1/memory>
//#include <boost/scoped_ptr.hpp>
//#include <tr1/functional>
//#include <iostream>
//#include <string>



namespace control {
  
//  using lumiera::Symbol;
//  using std::tr1::shared_ptr;
//  using boost::scoped_ptr;
//  using std::tr1::function;
//  using std::ostream;
//  using std::string;
  
  namespace { // ......................
    
  
  } // (END) impl details
  
  
  
  
  /**
   */
  template<typename SIG, class TAR, class BA>
  class ArgumentTupleAccept
    : BA
    {
    };
  
    
  
  
} // namespace control
#endif
