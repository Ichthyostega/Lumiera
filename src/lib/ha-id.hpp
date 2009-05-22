/*
  HA-ID.hpp  -  generic hash based and hierarchically typed ID
 
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


/** @file ha-id.hpp
 ** A template for generating hash based ID tags carrying compile-time type info.
 ** While the actual storage is assumed to be based on a POD, the type info is crucial
 ** to circumvent the problems with an "object" base class. Frequently, the need to
 ** manage some objects in a central facility drives us to rely on a common base class,
 ** irrespective of an actual common denominator in the semantics of the objects to
 ** be managed within this collection. Typically this results in this common base class
 ** being almost worthless as an API or interface, causing lots of type casts when using
 ** such a common object management facility. Passing additional context or API information
 ** on a metaprogramming level through the management interface helps avoiding these 
 ** shortcomings. 
 ** 
 ** Here we build an ID facility with the following properties:
 ** - based on a configurable storage/implementation of the actual hash or index code.
 ** - tied to a specific hierarchy of objects (template parameter "BA")
 ** - providing an additional template parameter to pass the desired type info
 ** - establishing an type hierarchy relation between ID template instances, such that
 **   the IDs typed to the derived/specialised objects can stand-in for the generic
 **   ID typed to the base class.
 ** - providing a Mixin, which allows any hierarchy to use this facility without 
 **   much code duplication.
 **
 ** @see HaID_test
 ** @see Placement usage example
 **
 */



#ifndef LIB_HA_ID_H
#define LIB_HA_ID_H

//#include "lib/util.hpp"

//#include <tr1/memory>
#include <cstdlib>


namespace lib {

//  using std::tr1::shared_ptr;
  using std::rand;
  
  
  struct LuidH 
    {
      long dummy_;
      
      LuidH() : dummy_(rand()) {}
    };
  
  /** 
   * Generic hash based and hierarchically typed ID
   * @todo WIP maybe also usable for assets?
   */
  template<typename T, class BA>
  struct HaID;
  
  template<class BA>
  struct HaIndexed
    {
      typedef HaID<BA,BA> RootID;
      RootID id_;
      
      RootID const& getID()  const
        { 
          return id_; 
        }
      void resetID(HaIndexed const& ref)
        { 
          this->id_ = ref.getID();
        }
      void resetID(RootID const& ref) ///< @todo this mutator should be removed in the final version to keep the actual hash opaque
        { 
          this->id_.dummy_ = ref.dummy_;
        }
    };
  
  template<class BA>
  struct HaID<BA,BA> : LuidH
    {
      HaID ()              : LuidH () {}
      HaID (BA const& ref) : LuidH (ref.getID()) {}
    };
  
  template<typename T, class BA>
  struct HaID : HaID<BA,BA>
    {
      HaID ()             : HaID<BA,BA> ()    {}
      HaID (T const& ref) : HaID<BA,BA> (ref) {}
    };
  
  
  
  
} // namespace lib
#endif
