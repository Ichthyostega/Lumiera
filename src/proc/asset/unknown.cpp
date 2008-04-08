/*
  Unknown  -  placeholder for unknown or unavailable media source
 
  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>
 
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


#include "proc/asset/unknown.hpp"
#include <boost/format.hpp>


using boost::format;

namespace asset
  {
  
  
  
  /** create a placeholder for a media with the given identity.
   *  the denoted original media (identity) can be accessed later
   *  on using the Unknown::getOrg() function.
   *  @note we don't depend on the refered media...
   */
  Unknown::Unknown (const Asset::Ident& idi, string name, Time length)
    : Media (idi, name, length)
  {
    TODO ("implement some sensible behaviour for the »unknown media« placeholder...");
  }  
  
  
  /** using the information stored in this placeholder asset, 
   *  try to access the "real" media it stands for.
   */
  Media::PMedia 
  Unknown::getOrg()  throw(cinelerra::error::Invalid)
  {
    UNIMPLEMENTED ("how to get at the original media from a »Unknown« placeholder");
    if (1==0)
      throw cinelerra::error::Invalid (str(format("Unable to locate original media "
                                                  "for ID=%s, filename=\"%s\".") 
                                                  % string(this->ident) 
                                                  % string(this->getFilename()))
                                      ,CINELERRA_ERROR_ORIG_NOT_FOUND
                                      );
  }
  
  CINELERRA_ERROR_DEFINE (ORIG_NOT_FOUND, "Media refered by placeholder not found");

  
  
} // namespace asset
