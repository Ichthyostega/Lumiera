/*
  ADVICE.hpp  -  generic loosely coupled interaction guided by symbolic pattern
 
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
 
*/


/** @file advice.hpp
 ** Expecting Advice and giving Advice: a cross-cutting collaboration of loosely coupled participants.
 ** TODO WIP-WIP
 ** 
 ** @note as of 4/2010 this is an experimental setup and implemented just enough to work out
 **       the interfaces. Ichthyo expects this collaboration service to play a central role
 **       at various places within proc-layer.
 ** 
 ** @see configrules.hpp
 ** @see typed-lookup.cpp corresponding implementation
 ** @see typed-id-test.cpp
 ** 
 */


#ifndef LIB_ADVICE_H
#define LIB_ADVICE_H


#include "lib/error.hpp"
//#include "proc/asset.hpp"
//#include "proc/asset/struct-scheme.hpp"
//#include "lib/hash-indexed.hpp"
//#include "lib/util.hpp"
#include "lib/symbol.hpp"

//#include <boost/operators.hpp>
//#include <tr1/memory>
//#include <iostream>
//#include <string>

namespace lib    {  ///////TODO: how to arrange the namespaces best?
namespace advice {
  
  /**
   * TODO type comment
   */
  template<class TY>
  class PointOfAdvice;
  

  
  
  
}} // namespace lib::advice
#endif
