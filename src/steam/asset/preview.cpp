/*
  Preview(Asset) -  alternative version of the media data, probably with lower resolution

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file preview.cpp
 ** Implementation details regarding a placeholder / "Proxy" / preview media asset
 */


#include "steam/asset/preview.hpp"

namespace steam {
namespace asset {
  
  
  namespace {
    /** @internal derive a sensible asset ident tuple when creating 
     *  a proxy placeholder media based on some existing asset::Media
     *  @todo getting this one right is important for the handling
     *        of "proxy editing"....
     */
    const Asset::Ident
    createProxyIdent (const Asset::Ident& mediaref)
    {
      string name (mediaref.name + "-proxy");  // TODO something sensible here; append number, sanitise etc.
      Category category (mediaref.category);
      TODO ("put it in another subfolder within the same category??");
      return Asset::Ident (name, category, 
                           mediaref.org, 
                           mediaref.version );
      
    }
  }//(End)implementation helper
  
  
  
  /** create a preview placeholder ("proxy media") for the given
   *  media asset. The name of the created media asset is derived
   *  by decorating the original media's name.
   *  @note creates a dependency between the media and this proxy
   */
  Preview::Preview (Media& mediaref)
    : Unknown (createProxyIdent (mediaref.ident),
               mediaref.getFilename(),
               mediaref.getLength())
  {
    this->defineDependency (mediaref);
    UNIMPLEMENTED ("do something to setup proxy media");
  }
  
  
  
  
}} // namespace asset
