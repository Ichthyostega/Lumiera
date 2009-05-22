/*
  PLACEMENT-REF.hpp  -  generic reference to an individual placement added to the session
 
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


/** @file placement-ref.hpp 
 **
 ** @see Placement
 ** @see PlacementRef_test
 **
 */



#ifndef MOBJECT_PLACEMENT__REF_H
#define MOBJECT_PLACEMENT__REF_H

//#include "pre.hpp"
//#include "proc/mobject/session/locatingpin.hpp"
//#include "proc/asset/pipe.hpp"

//#include <tr1/memory>


namespace mobject {

//  using std::tr1::shared_ptr;
  
  
  /** @todo WIP a generic hash-index, maybe also usable for assets */
  struct LuidH 
    {
      long dummy_;
    };
  
  template<typename T, class BA>
  struct HaID;
  
  template<class BA>
  struct HaIndexed
    {
      LuidH id_;
      HaID<BA,BA> const& getID()  const;
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
  
  template<class BA>
  inline HaID<BA,BA> const&
  HaIndexed<BA>::getID()  const
  {
    return *(static_cast<const HaID<BA,BA>*> (&id_));
  }

  
  struct Base
    {
      int ii_;
    };
  
  struct TestA : Base, HaIndexed<TestA>
    {
    };
  struct TestBA : TestA {};
  struct TestBB : TestA {};

  
  
  /**
   */
  class PlacementRef
    {
    public:
      /**
       * 
       */
      class ID
        {
        };
      
      template<class MO>
      class IDp : public ID
        {
        };
      
      
    };
  
  
  
  
  


} // namespace mobject
#endif
