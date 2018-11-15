/*
  Preview(Asset) -  alternative version of the media data, probably with lower resolution

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

* *****************************************************/


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
