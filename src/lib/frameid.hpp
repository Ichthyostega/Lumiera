/*
  FRAMEID.hpp  -  distinct identification of a single data frame
 
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


#ifndef LUMIERA_FRAMEID_H
#define LUMIERA_FRAMEID_H



namespace lumiera
  {


  /**
   * Identification tuple for addressing frames unambiguously.
   * 
   * @todo currently (7/08) this is a dummy implementation to find out
   *       what interface the Proc layer needs. Probably the backend will
   *       later on define what is actually needed; this header should then
   *       be replaced by a combined C/C++ header
   */
  class FrameID : boost::totally_ordered<FrameID>  ////////////TODO it seems we don't need total ordering, only comparison. Clarify this!
    {
      long dummy;
    public:
      FrameID(long dum=0) : dummy(dum) {}
      operator long () { return dummy; }
      
      bool operator<  (const FrameID& of)  const { return dummy <  of.dummy; }
      bool operator== (const FrameID& of)  const { return dummy == of.dummy; }
    };
    
  

   
} // namespace lumiera
#endif
