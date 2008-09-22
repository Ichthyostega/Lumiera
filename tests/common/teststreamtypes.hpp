/*
  TESTSTREAMTYPES.hpp  -  create test (stub) stream type information
 
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


#ifndef LUMIERA_TEST_TESTSTREAMTYPES_H
#define LUMIERA_TEST_TESTSTREAMTYPES_H


//#include "common/util.hpp"

#include "common/streamtype.hpp"
#include "proc/control/stypemanager.hpp"



namespace lumiera {
  namespace test {
    
    /** Helper: create an implementation frame
     *  and build the corresponding streamtype
     */
    inline StreamType::ImplFacade&
    createImplType ()
    {
      UNIMPLEMENTED ("create a test stream type from a given GAVL type tag");
    }
    
    
    
  } // namespace test

} // namespace lumiera
#endif
