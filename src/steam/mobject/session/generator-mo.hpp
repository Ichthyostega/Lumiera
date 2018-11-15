/*
  GENERATOR-MO.hpp  -  a (Test)data generator

  Copyright (C)         Lumiera.org
    2011,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file generator-mo.hpp
 ** A data generator media object.
 ** Can be used as placeholder, or as testing device
 ** @todo WIP implementation of player subsystem from 2011
 ** @todo as of 2016, this effort is considered stalled but basically valid
 */


#ifndef PROC_MOBJECT_SESSION_GENERATOR_MO_H
#define PROC_MOBJECT_SESSION_GENERATOR_MO_H

#include "proc/mobject/session/abstractmo.hpp"
#include "lib/time/timevalue.hpp"


namespace proc {
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
  
  
  
}}} // namespace proc::mobject::session
#endif
