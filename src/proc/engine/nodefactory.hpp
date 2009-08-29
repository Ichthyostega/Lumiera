/*
  NODEFACTORY.hpp  -  Interface for creating processing nodes of various kinds
 
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


#ifndef ENGINE_NODEFACTORY_H
#define ENGINE_NODEFACTORY_H

#include "proc/engine/procnode.hpp"
#include "proc/mobject/placement.hpp"



namespace mobject {
namespace session {
  
  class Clip;
  class Effect;
  typedef Placement<Effect> PEffect;
  // TODO: class Transition;
  
}} // namespace mobject::session


namespace engine {

  using std::vector;
  
  class Trafo;
  typedef Trafo* PTrafo;    ///< @note ProcNode is handled by pointer and bulk allocated/deallocated 


  /**
   * Create processing nodes based on given objects of the high-level model.
   */
  class NodeFactory
    {
      
      public:
        
        PTrafo  operator() (mobject::session::PEffect const&);
        
    };

} // namespace engine
#endif
