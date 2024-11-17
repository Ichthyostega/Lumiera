/*
  BINDING.hpp  -  link to use a sequence within the session

   Copyright (C)
     2009,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file session/binding.hpp
 ** MObject in session to represent the top-level binding of a sequence
 ** @todo stalled effort towards a session implementation from 2010
 ** @todo 2016 likely to stay, but expect some extensive rework
 ** @todo Also consider timeline-slave, a topic postponed as of 10/2018     /////////////////////////////////TICKET #1083
 */


#ifndef STEAM_MOBJECT_SESSION_BINDING_H
#define STEAM_MOBJECT_SESSION_BINDING_H

#include "steam/mobject/session/meta.hpp"
#include "steam/mobject/builder/buildertool.hpp"


namespace steam {
namespace asset { 
  class Sequence;
  
  typedef lib::P<Sequence> PSequence;

}

namespace mobject {
namespace session {
  
    using asset::PSequence;
    
    /**
     * Explicit link to bind a Sequence (container) to be used within the Session,
     * either as contents of a top level Timeline, or as embedded MetaClip.
     * Usually, the Placement holding such a Binding also constitutes a scope
     * on its own, containing other nested objects.
     */
    class Binding : public Meta
      {
        PSequence boundSequence_;
        
        string
        initShortID()  const
          {
            return buildShortID("Binding");
          }
        
        bool isValid()  const;
        
      public:
        Binding (PSequence const& sequence_to_bind);
          
        DEFINE_PROCESSABLE_BY (builder::BuilderTool);

      };
    
  } // namespace mobject::session
  
  /** Placement<Binding> defined to be subclass of Placement<Meta> */
  template class Placement<session::Binding, session::Meta>;
  typedef Placement<session::Binding, session::Meta> PBinding;
  
}} // namespace steam::mobject
#endif
