/*
  PROCPATT.hpp  -  template for building some render processing network
 
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


#ifndef ASSET_PROCPATT_H
#define ASSET_PROCPATT_H

#include "proc/asset/struct.hpp"

#include <vector>

using std::vector;



namespace asset
  {
  
  class BuildInstruct;


  /**
   * special type of structural Asset 
   * representing information how to build some part 
   * of the render engine's processing nodes network.
   */
  class ProcPatt : public Struct
    {
    protected:
      const vector<BuildInstruct> instructions;

    };
    
    
    
} // namespace asset
#endif
