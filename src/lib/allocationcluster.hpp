/*
  ALLOCATIONCLUSTER.hpp  -  allocating and owning a pile of objects 
 
  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>
 
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

/** @file allocationcluster.hpp
 ** Memory management for the low-level model (render nodes network).
 ** The model is organised into temporal segments, which are considered
 ** to be structurally constant and uniform. The objects within each
 ** segment are strongly interconnected, and thus each segment is 
 ** being built in a single build process and is replaced or released
 ** as a whole. AllocationCluster implements memory management to
 ** support this usage pattern. 
 **
 ** @see allocationclustertest.cpp
 ** @see builder::ToolFactory
 ** @see frameid.hpp
 */


#ifndef LUMIERA_APPCONFIG_H
#define LUMIERA_APPCONFIG_H

//#include <map>
#include <string>
//#include <boost/scoped_ptr.hpp>
#include <boost/noncopyable.hpp>



namespace lib {
  using std::string;
//  using boost::scoped_ptr;


  /**
   * A pile of objects sharing common allocation and lifecycle.
   * AllocationCluster owns a number of object families of various types.
   * Each of those contains a initially undetermined (but rather large)
   * number of individual objects, which can be expected to be allocated
   * within a short timespan and which are to be released cleanly on
   * destruction of the AllocationCluster. There is a service creating
   * individual objects with arbitrary ctor parameters and it is possible
   * to control the oder in which the object families are to be discarded.
   * @warning make sure the objects dtors aren't called and object references
   *          aren't used after shutting down a given AllocationCluster.
   */
  class AllocationCluster
    : boost::noncopyable
    {
      
    public:
      AllocationCluster ();
      ~AllocationCluster ()  throw();
      
      template<class TY>
      TY&
      create ()
        {
          TY* obj = new(allocation<TY>()) TY();
          return success(obj);
        }
      
      template<class TY, typename P0>
      TY&
      create (P0& p0)
        {
          TY* obj = new(allocation<TY>()) TY (p0);
          return success(obj);
        }
      
      template<class TY, typename P0, typename P1>
      TY&
      create (P0& p0, P1& p1)
        {
          TY* obj = new(allocation<TY>()) TY (p0,p1);
          return success(obj);
        }
      
      template<class TY, typename P0, typename P1, typename P2>
      TY&
      create (P0& p0, P1& p1, P2& p2)
        {
          TY* obj = new(allocation<TY>()) TY (p0,p1,p2);
          return success(obj);
        }
      
      template<class TY, typename P0, typename P1, typename P2, typename P3>
      TY&
      create (P0& p0, P1& p1, P2& p2, P3& p3)
        {
          TY* obj = new(allocation<TY>()) TY (p0,p1,p2,p3);
          return success(obj);
        }

      
    protected:
      /** initiate an allocation for the given type */
      template<class TY>
      void*
      allocation();
      
      /** finish the allocation after the ctor is successful */
      template<class TY>
      TY&
      success (TY* obj);
    };
    
  



} // namespace lib
#endif
