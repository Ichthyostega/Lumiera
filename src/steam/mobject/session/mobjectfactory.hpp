/*
  MOBJECTFACTORY.hpp  -  creating concrete MObject subclass instances

  Copyright (C)         Lumiera.org
    2008, 2010,         Hermann Vosseler <Ichthyostega@web.de>

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


/** @file mobjectfactory.hpp
 ** Core factory to generate media objects for use in the Session model.
 ** @todo stalled effort towards a session implementation from 2008
 ** @todo 2016 likely to stay, but expect some extensive rework.
 **       In fact I am quite unhappy with the shape of this code
 */


#ifndef STEAM_MOBJECT_SESSION_MOBJECTFACTORY_H
#define STEAM_MOBJECT_SESSION_MOBJECTFACTORY_H

#include "steam/mobject/mobject.hpp"
#include "lib/idi/entry-id.hpp"
#include "lib/symbol.hpp"


namespace lumiera {
namespace query {
  class DefsManager;
}}

namespace steam {
namespace asset {
  
  class Clip;
  class Media;
  class Effect;
  class Sequence;
  
  typedef lib::P<Sequence> PSequence;
  
  }

namespace mobject {
namespace session {
  
  class Root;
  class Clip;
  class Fork;
  class Effect;
  class Label;
  class Binding;
  
  
  
  class MObjectFactory
    {
      /** custom deleter function allowing
       *  Placement to take ownership of MObjct instances
       */
      static void deleterFunc (MObject* o) { delete o; }
      
    public:
      
      Placement<Root>   operator() (lumiera::query::DefsManager&);
      Placement<Clip>   operator() (asset::Clip const&, asset::Media const&);
      Placement<Clip>   operator() (asset::Clip const&, vector<asset::Media const*>);
      Placement<Fork>   operator() (lib::idi::EntryID<Fork> const&);
      Placement<Effect> operator() (asset::Effect const&);
      Placement<Label>  operator() (lib::Symbol);
      Placement<Binding>operator() (asset::PSequence const&);
      
      ////////////////////////////////////////////////////////////////////////////////TICKET #414
    };
  
  
  
}}} // namespace steam::mobject::session
#endif
