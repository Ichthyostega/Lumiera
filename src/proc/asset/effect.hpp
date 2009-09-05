/*
  EFFECT.hpp  -  Effect or media processing component
 
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


#ifndef ASSET_EFFECT_H
#define ASSET_EFFECT_H

#include "proc/asset/proc.hpp"



namespace asset
  {
  
  /**
   * Effect or media processing component
   */
  class Effect : public Proc
    {
    
    public:
      /** resolve any plugin and configuration info
       *  to yield the actual media data processing function.
       *  @return a function ready to be invoked; either the 
       *          "real thing" or a suitable placeholder.
       *  @throw lumiera::error::Fatal if unable to provide
       *         any usable function or placeholder. This case
       *         can be considered exceptional and justifies a
       *         subsystem failure.
       */
      ProcFunc*
      resolveProcessor();
      

    };
    
    
    
} // namespace asset
#endif
