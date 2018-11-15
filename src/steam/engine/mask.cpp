/*
  Mask  -  Video ProcNode for masking regions of the image (automatable)

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

* *****************************************************/


/** @file mask.cpp
 ** Mask generator node implementation
 ** @deprecated not clear if needed as of 2016
 */


#include "lib/error.hpp"
#include "steam/engine/mask.hpp"

namespace proc {
namespace engine {
  
  
  Mask::Mask (WiringDescriptor const& wd)
    : ProcNode(wd)
    {
      UNIMPLEMENTED("do we need a dedicated ProcNode subclass as mask generator?");
    }
  
  /** */
  
  
  
}}// namespace engine
