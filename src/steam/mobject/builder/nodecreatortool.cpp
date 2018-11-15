/*
  NodeCreaterTool  -  central Tool implementing the Renderengine building

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


/** @file nodecreatortool.cpp
 ** Implementation of visitor logic to actually build the render node network
 ** @todo stalled design draft from 2008 -- basically still considered relevant as of 2016
 */


#include "lib/error.hpp"
#include "steam/mobject/builder/nodecreatortool.hpp"



namespace steam {
namespace mobject {
namespace builder {
  
  using mobject::Buildable;
  using mobject::session::Clip;
  using mobject::session::Effect;
  using mobject::session::Auto;
  
                                      /////////////////////////////////TICKET #414
  
  
  ////TODO: do we ever get to treat a model root element??
  
  
  void
  NodeCreatorTool::treat (Buildable& something)
  {
    UNIMPLEMENTED ("the builder tool...");
  }


  void
  NodeCreatorTool::treat (Clip& clip)
  {
    UNIMPLEMENTED ("the builder tool...");
  }


  void
  NodeCreatorTool::treat (Effect& effect)
  {
    UNIMPLEMENTED ("the builder tool...");
  }


  void
  NodeCreatorTool::treat (Auto<double>& automation)
  {
    UNIMPLEMENTED ("the builder tool...");
  }
  
  
  void
  NodeCreatorTool::onUnknown (Buildable& target)
  {
    UNIMPLEMENTED ("catch-all when partitioning timeline");  ////////TODO: verify why this gets enforced here...
  }



}}} // namespace steam::mobject::builder
