/*
  TRAFO.hpp  -  transforming processing Node 

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


#ifndef ENGINE_TRAFO_H
#define ENGINE_TRAFO_H

#include "proc/engine/procnode.hpp"



namespace engine
  {


  /**
   * abstraction of the most important kind of Processing node,
   * which really works on the media data and transforms input
   * into ouput. Subclasses include the (Video) Projector
   * for scaling/translating, all sorts of effects (Plugins),
   * as well as the low level codecs used to decode the raw
   * media at the source end of the render pipeline(s)
   */
  class Trafo : public ProcNode
    {
    protected:
      Trafo (WiringDescriptor const& wd)
        : ProcNode(wd)
        { }
      
      friend class NodeFactory;
      
      
      
      /** do the actual calculations.
       *  @internal dispatch to implementation. 
       *            Client code should use #render()
       *  @todo obviously we need a parameter!!!
       */
      virtual void process() = 0;
    };

  typedef Trafo* PTrafo;    ///< @todo handle ProcNode by pointer or by shared-ptr??
  


} // namespace engine
#endif
