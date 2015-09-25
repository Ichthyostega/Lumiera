/*
  HIERARCHY-ORIENTATION-INDICATOR.hpp  -  helper to mark level on tree navigation

  Copyright (C)         Lumiera.org
    2013,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file hierarchy-orientation-indicator.hpp
 ** Helper to support navigating a tree structure.
 ** The OrientationIndicator records reference levels (depth into the tree)
 ** and can then be used to determine the relative orientation between the
 ** previously marked reference level and the current reference level.
 ** This simple state capturing mechanism can be used to track the path
 ** of a tree visitation, or to sync an external stack with a currently
 ** investigated tree level.
 ** 
 ** The relative orientation value can be retrieved through an int conversion;
 ** to ease recursive programming, this statefull value can be incremented and
 ** decremented without influencing the captured reference level. 
 ** 
 ** @see job-ticket.hpp usage example
 ** @see HierarchyOrientationIndicator_test
 ** 
 */


#ifndef HIERARCHY_ORIENTATION_INDICATOR_H
#define HIERARCHY_ORIENTATION_INDICATOR_H


#include <cstddef>
#include <limits>
#include <string>

namespace lib {
  
  using std::string;
  
  
  class OrientationIndicator
    {
      size_t refLevel_;
      ptrdiff_t offset_;
      
    public:
      OrientationIndicator()
        : refLevel_(0)
        , offset_(0)
        { }
      
      // using default copy/assignment
      
      operator ptrdiff_t()  const
        {
          return offset_;
        }
      
      
      
      void
      markRefLevel (size_t newRefLevel)
        {
          REQUIRE (newRefLevel < size_t(std::numeric_limits<ptrdiff_t>::max()) );
          
          offset_ -= newRefLevel - refLevel_;
          refLevel_ = newRefLevel;
        }
      
      /** define the current offset as new reference point */
      OrientationIndicator&
      markRefLevel ()
        {
          REQUIRE (0 < ptrdiff_t(refLevel_) + offset_);
          
          markRefLevel (refLevel_ + offset_);
          ENSURE (offset_ == 0);
          return *this;
        }
      
      OrientationIndicator&
      resetToRef ()
        {
          offset_ = 0;
          return *this;
        }
      
      OrientationIndicator&
      operator+= (int adj)
        {
          offset_ += adj;
          return *this;
        }
      
      OrientationIndicator&
      operator-= (int adj)
        {
          offset_ -= adj;
          return *this;
        }
      
      OrientationIndicator&
      operator++ ()
        {
          this->operator +=(1);
          return *this;
        }
      
      OrientationIndicator&
      operator-- ()
        {
          this->operator -=(1);
          return *this;
        }
    };
  
  
  
} // namespace lib
#endif /*HIERARCHY_ORIENTATION_INDICATOR_HPP_*/
