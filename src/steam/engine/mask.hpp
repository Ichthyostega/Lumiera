/*
  MASK.hpp  -  Video ProcNode for masking regions of the image (automatable)

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file mask.hpp
 ** Specialised render node for generating mask (alpha channel) data.
 ** @todo this file is there since the very first code generation steps at start
 **       of the Lumiera project. It never became clear if a mask generator would
 **       need to be a dedicated ProcNode subclass; but in fact the project did
 **       not reach the point of implementing anything regarding that topic.
 */


#ifndef STEAM_ENGINE_MASK_H
#define STEAM_ENGINE_MASK_H

#include "steam/engine/procnode.hpp"



namespace steam {
namespace engine{
  
  
  class Mask
    : public ProcNode
    {
    public:
      Mask (WiringDescriptor const&);
    };
  
  
  
}} // namespace steam::engine
#endif /*STEAM_ENGINE_MASK_H*/
