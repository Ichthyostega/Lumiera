/*
  MediaAccessFacade  -  functions for querying media file and channels.

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file media-access-facade.cpp
 ** Implementation binding to query for a media file's properties.
 ** 
 ** @todo this was added as a draft and mock implementation in 2008
 **       and is only used sporadic for unit tests.
 */


#include "vault/media-access-facade.hpp"

#include "lib/util.hpp"

using util::isnil;
using lumiera::error::Invalid;

namespace vault {
  
  /** storage for the SingletonFactory 
   *  (actually a lumiera::test::MockInjector) */
  lib::Depend<MediaAccessFacade> MediaAccessFacade::instance;

  
  
  MediaDesc& 
  MediaAccessFacade::queryFile (string const& name)  const
  {
    if (isnil (name))
      throw Invalid ("empty filename passed to MediaAccessFacade.");
    
    UNIMPLEMENTED ("delegate to vault: query accessability of file");
  }
  
  
  ChanDesc 
  MediaAccessFacade::queryChannel (MediaDesc& mHandle, uint chanNo)  const
  {
    UNIMPLEMENTED ("delegate to vault: query channel information");
    ChanDesc nix;
    return nix;
  }



} // namespace vault
