/*
  ENTRY-ID.hpp  -  plain symbolic and hash ID used for accounting
 
  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>
 
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
 
*/


/** @file entry-id.hpp
 ** Bare symbolic and hash ID used for accounting of asset like entries.
 ** This ID can be used to register instances with an accounting table, without all the
 ** overhead of creating individual assets for each entry. The datafields in the symbolic part
 ** of the ID are similar to the asset identity tuple; the idea is to promote individual entries
 ** to full fledged assets on demand. Alongside with the symbolic identity, which can be reduced
 ** to just a Symbol and a type identifier, we store the derived hash value as LUID. 
 ** 
 ** @note as of 3/2010 this is an experimental setup and exists somewhat in parallel
 **       to the assets. We're still in the process of finding out what's really required
 **       to keep track of all the various kinds of objects.
 **
 ** @see asset::Asset::Ident
 ** @see entry-id-test.cpp
 ** 
 */


#ifndef ASSET_ENTRY_ID_H
#define ASSET_ENTRY_ID_H


#include "proc/asset.hpp"



namespace asset {
  
  
  /** 
   * thin ID with blah
   * 
   * @see mobject::session::Track
   */
  template<class KIND>
  class ID
    {
    public:
    };
    
    
    

} // namespace asset
#endif
