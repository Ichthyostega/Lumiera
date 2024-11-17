/*
  GENERATOR-MO.hpp  -  a (Test)data generator

   Copyright (C)
     2011,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file generator-mo.hpp
 ** A data generator media object.
 ** Can be used as placeholder, or as testing device
 ** @todo WIP implementation of player subsystem from 2011
 ** @todo as of 2016, this effort is considered stalled but basically valid
 */


#ifndef STEAM_MOBJECT_SESSION_GENERATOR_MO_H
#define STEAM_MOBJECT_SESSION_GENERATOR_MO_H

#include "steam/mobject/session/abstractmo.hpp"
#include "lib/time/timevalue.hpp"


namespace steam {
namespace asset {
  class Media;
  class Clip;
}

namespace mobject {
namespace session {
  
  using asset::Media;
  using lib::time::TimeVar;
  
  typedef lib::P<Media> PMedia;
  typedef lib::P<asset::Clip> PClipAsset;
  
  
  /**
   * A lksjaf
   */
  class GeneratorMO
    : public AbstractMO
    {
      string
      initShortID()  const
        {
          return buildShortID("Generator");
        }
      
      void setupLength();
      
      
      
    protected:
      /** start position in source */
      TimeVar start_;
      
      /** @todo using a mere ref here is against the scheme and only
          done as temporal solution, until we work out how to handle
          multichannel clips. It should be a smart pointer of some kind
          and the unlink() function of the asset should take it into
          account when breaking circular references.
       */
      
      
      GeneratorMO ();
      friend class MObjectFactory;
      
      
    public:
      bool isValid()  const;
      
      
      DEFINE_PROCESSABLE_BY (builder::BuilderTool);
      
    };
  
  
  
}}} // namespace steam::mobject::session
#endif
