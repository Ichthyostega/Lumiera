/*
  ITER-EXPLORER.hpp  -  building blocks for iterator evaluation strategies 

  Copyright (C)         Lumiera.org
    2012,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file iter-explorer.hpp
 ** Helper template(s) for establishing various evaluation strategies for hierarchical data structures.
 ** Based on the <b>Lumiera Forward Iterators</b> concept and using the basic IterAdaptor templates,
 ** these components allow to implement typical evaluation strategies, like e.g. depth-first or
 ** breadth-first exploration of a hierarchical structure. Since the access to this structure is
 ** abstracted through the underlying iterator, what we effectively get is a functional datastructure.
 ** The implementation is based on the IterableStateWrapper, which is one of the basic helper templates
 ** provided by iter-adapter.hpp.
 ** 
 ** @see IterExplorer_test.cpp
 ** @see iter-adapter.hpp
 ** @see itertools.hpp
 ** @see IterSource (completely opaque iterator)
 **
 */


#ifndef LIB_ITER_EXPLORER_H
#define LIB_ITER_EXPLORER_H


#include "lib/error.hpp"
//#include "lib/bool-checkable.hpp"
#include "lib/iter-adapter.hpp"

//#include <boost/type_traits/remove_const.hpp>


namespace lib {
  
  
  namespace { // internal helpers
  }
  
  

  /**
   * Adapter for building....
   * TODO write type comment
   */
  template<class POS, class CON>
  class IterExplorer
//    : public lib::BoolCheckable<IterAdapter<POS,CON> >
    {
//      CON source_;
//      mutable POS pos_;
      
    public:
//      typedef typename iter::TypeBinding<POS>::pointer pointer;
//      typedef typename iter::TypeBinding<POS>::reference reference;
//      typedef typename iter::TypeBinding<POS>::value_type value_type;
      
      
    protected: /* === xxx === */
      
      /** */ 
      
    private:
    };
  
  
  
} // namespace lib
#endif
