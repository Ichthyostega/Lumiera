/*
  Preview(Asset) -  alternative version of the media data, probably with lower resolution
 
  Copyright (C)         CinelerraCV
    2007,               Christian Thaeter <ct@pipapo.org>
 
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
 
* *****************************************************/


#include "proc/asset/preview.hpp"

namespace asset
  {
  
  
  namespace
    {
      /** @internal derive a sensible asset ident tuple when creating 
       *  a proxy placeholder media based on some existing asset::Media
       *  @todo getting this one right is important for the handling
       *        of "proxy editing"....
       */
      const Asset::Ident
      createProxyIdent (const Asset::Ident& mediaref)
        {
          string name (mediaref.name + "-proxy");  // TODO something sensible here; append number, sanitize etc.
          Category category (mediaref.category);
          TODO ("put it in another subfolder within the same category??");
          return Asset::Ident (name, category, 
                               mediaref.org, 
                               mediaref.version );
          
        }
    }
  
  
  
  /** create a preview placeholder ("proxy media") for the given
   *  media asset 
   */
  Preview::Preview (const Media& mediaref)
    : Media (createProxyIdent (mediaref.ident),
             mediaref.getFilename(),
             mediaref.getLength())
  {
    UNIMPLEMENTED ("do something to setup proxy media");
  }
  
  
  
  /** create a dummy placeholder
   *  @internal for use by asset::Unknown
   *  @todo better design!
   */
  Preview::Preview (const Asset::Ident& idi, string name, Time length)
    : Media (createProxyIdent (idi),
             name, length)
  {
    TODO ("work out how to handle unknown media placheholder");
  }




} // namespace asset
