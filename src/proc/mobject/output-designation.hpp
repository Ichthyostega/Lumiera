/*
  OUTPUT-DESIGNATION.hpp  -  specifying a desired output destination
 
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


#ifndef PROC_MOBJECT_OUTPUT_DESIGNATION_H
#define PROC_MOBJECT_OUTPUT_DESIGNATION_H

#include "proc/asset/pipe.hpp"


namespace mobject {
  
  
  
  /**
   * Descriptor to denote the desired target of produced media data.
   * OutputDesignation is always an internal and relative specification
   * and boils down to referring a asset::Pipe by ID. In order to get
   * actually effective, some object within the model additionally
   * needs to \em claim this pipe-ID, meaning that this object
   * states to root and represent this pipe. When the builder
   * encounters a pair of (OutputDesignation, OutputClaim),
   * an actual stream connection will be wired in the
   * processing node network.
   */
  class OutputDesignation
    {
    public:
    };
  
  
  
} // namespace mobject
#endif
