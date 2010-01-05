/*
  ITER-SOURCE.hpp  -  an interface to build an opaque iterator-based data source 
 
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

/** @file iter-source.hpp
 ** Extension module to build an opaque data source, accessible as
 ** <i>Lumiera Forward Iterator</i>. It is based on combining an IterAdapter
 ** with classical polymorphism; here, the data source, which is addressed
 ** by IderAdapter through the "iteration control API", is abstracted behind
 ** an interface (with virtual functions). Together this allows to build
 ** a simple data source type, without needing to disclose details of
 ** the implementation.
 ** 
 ** @see iter-adapter.hpp
 ** @see itertool.hpp
 ** @see iter-source-test.cpp
 ** 
 */


#ifndef LIB_ITER_SOURCE_H
#define LIB_ITER_SOURCE_H


#include "lib/iter-adapter.hpp"
//#include "lib/bool-checkable.hpp"




namespace lib {
  
  
  namespace {
  }
  
  
  /**
   * Iteration source interface to abstract a data source,
   * which then can be accessed through IterAdapter as a frontend,
   * allowing to pull individual elements until exhaustion.
   * 
   * @see PlacementIndex::Table#_eachEntry_4check usage example
   * @see iter-source-test.cpp
   */
  template<class TY>
  class IterSource
    : public lib::BoolCheckable<IterSource<TY> >
    {
    };
  
  
  
} // namespace lib
#endif
