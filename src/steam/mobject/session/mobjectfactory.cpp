/*
  MObjectFactory  -  creating concrete MObject subclass instances

   Copyright (C)
    2008,2010,          Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file mobjectfactory.cpp
 ** @todo stalled effort towards a session implementation from 2008
 ** @todo 2016 likely to stay, but expect some extensive rework
 */


#include "steam/mobject/session/mobjectfactory.hpp"
#include "steam/mobject/session/root.hpp"
#include "steam/mobject/session/clip.hpp"
#include "steam/mobject/session/fork.hpp"
#include "steam/mobject/session/effect.hpp"
#include "steam/mobject/session/label.hpp"
#include "steam/mobject/session/binding.hpp"
#include "steam/asset/clip.hpp"
#include "steam/asset/effect.hpp"
#include "common/query/defs-manager.hpp"

using lumiera::query::DefsManager;

namespace steam {
namespace mobject {
namespace session {
    ////////////////////////////////////////////////////////////////////////////////TICKET #414
  
  
  
  /** build a new session/model root element. */
  Placement<Root>
  MObjectFactory::operator() (DefsManager& sessionDefaultsHandler)
  {
    return Placement<Root> (*new Root (sessionDefaultsHandler), &deleterFunc);
  }
  
  
  /** build a new session/model root element. */
  Placement<Label>
  MObjectFactory::operator() (Symbol labelType)
  {
    return Placement<Label> (*new Label (labelType), &deleterFunc);
  }
  
  
  /** creating a Clip-MObject to be placed within the Session,
   *  based on a clip asset, which typically is obtained by calling
   *  the createClip()-function on some asset::Media.
   *  The newly created Clip-MO is (back)linked with this media Asset
   *  internally and wrapped into a Placement, which takes ownership.
   *  So, when the render engine gets across this Clip-MO, it is able
   *  to obtain the media information contained in the corresponding
   *  media asset.
   *  @param mediaDef the actual asset::Media to be used 
   *  created Clip could be a compound (multichannel) clip 
   *  comprised of several SimpleClip sub-objects.
   */
  Placement<Clip>   
  MObjectFactory::operator() (const asset::Clip& clipRef, const asset::Media& mediaDef)
  {
    return Placement<Clip> (*new Clip (clipRef,mediaDef), &deleterFunc);
  }
  
  /** variant of the Clip-MO factory function, creating
   *  a multichannel (compound) clip.
   *  @todo work out the details of multichannel handling
   */
  Placement<Clip>
  MObjectFactory::operator() (const asset::Clip& clipRef, vector<const Media*> mediaTracks)
  {
    UNIMPLEMENTED ("build multichannel compound clip-MO");
  }
  
  /** fabricate a new fork-MObject, using the given unique ID */
  Placement<Fork>
  MObjectFactory::operator() (ForkID const& id)
  {
    TODO ("what needs to be registered when creating an Fork-MO?");
    TODO ("assure the ID is indeed unique and not already used for another fork??");  //////////TICKET #638
    return Placement<Fork> (*new Fork (id), &deleterFunc);
  }
  
  /** */ 
  Placement<Effect>
  MObjectFactory::operator() (const asset::Effect& processorDef)
  {
    UNIMPLEMENTED ("fabricate effect-MO");
  }
  
  /** */ 
  Placement<Binding>
  MObjectFactory::operator() (PSequence const& sequence_to_bind)
  {
    TODO ("any additional liabilities when creating a binding?");
    return Placement<Binding> (*new Binding(sequence_to_bind), &deleterFunc);
  }
  
  
  
}}} // namespace mobject::session
