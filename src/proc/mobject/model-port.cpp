/*
  ModelPort  -  point to pull output data from the model
 
  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>
 
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
 
* *****************************************************/


/** @file model-port.cpp 
 ** Implementation details of model port descriptors and references.
 ** Especially, the handling of the ModelPortTable datastructure is
 ** kept an opaque implementation detail and confined entirely within
 ** this translation unit.
 ** 
 ** TODO: comment necessary?
 ** 
 ** @see OutputDesignation
 ** @see OutputMapping
 ** @see Timeline
 **
 */


#include "lib/error.hpp"
//#include "lib/symbol.hpp"//
#include "proc/mobject/model-port.hpp"

//#include <boost/functional/hash.hpp>
//#include <cstdlib>
#include <boost/noncopyable.hpp>

//using lumiera::query::QueryHandler;
//using lumiera::query::removeTerm;
//using lumiera::query::extractID;
//using lumiera::ConfigRules;

//using lumiera::Symbol;

namespace mobject {
  
  
  /**
   * TODO type comment
   */
  class ModelPortTable
    : boost::noncopyable
    {
      
    public:
    };
  
  
  
} // namespace mobject
