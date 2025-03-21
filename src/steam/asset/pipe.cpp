/*
  Pipe  -  structural asset denoting a processing pipe to generate media output

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file pipe.cpp
 ** Implementation details regarding the Asset representation of a processing Pipe
 */


#include "steam/asset/pipe.hpp"
#include "steam/assetmanager.hpp"
#include "lib/util.hpp"

using util::isnil;

namespace steam {
namespace asset {
  
  
  /** Create and register a new Pipe asset.
   *  Usually, this is triggered automatically
   *  by referring to the pipeID. When building
   *  the render network, the given processing pattern
   *  will be executed, allowing for all sorts of
   *  default wiring.
   */
  Pipe::Pipe ( const Asset::Ident& idi
             , string const& streamID     ////////////////////////////////////////TICKET #648
             , PProcPatt& wiring
             , string shortName
             , string longName
             )
    : Struct (idi)
    , wiringTemplate_(wiring)
    , streamID_(streamID)
    , shortDesc (shortName)
    , longDesc (longName)
  {
    REQUIRE (idi.isValid());
    if (isnil (shortDesc))
      shortDesc = string(idi);
  }
  
  
  
  PPipe 
  Pipe::query (string const& properties)
  { 
    return Struct::retrieve (Query<Pipe> (properties));
  }
  
  
  /** @param id asset-ID of the pipe to retrieve
   *  @throw error::Invalid when not found
   */ 
  PPipe
  Pipe::lookup (ID<Pipe> id)
  {
    return AssetManager::instance().getAsset(id);
  }

  
  void 
  Pipe::switchProcPatt (PProcPatt& another)
  {
    wiringTemplate_ = another;
    TODO ("trigger rebuild fixture");
  }
  
  
  
  
}} // namespace asset
