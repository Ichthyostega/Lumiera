/*
  Port  -  structural asset corresponding to some port generating media output
 
  Copyright (C)         CinelerraCV
    2007,               Hermann Vosseler <Ichthyostega@web.de>
 
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


#include "proc/asset/port.hpp"

namespace asset
  {
  
  namespace // Port Asset implementation details
    {
      /** @internal derive a sensible asset ident tuple when creating 
       *  a port asset based on a query
       *  @todo define the actual naming scheme of struct assets
       */
      const Asset::Ident
      createPortIdent (PProcPatt& wiring, string& id, wstring& shortD, wstring& longD)
        {
          string name ("port-" + id);  // TODO something sensible here; append number, sanitize etc.
          TODO ("Implement port name scheme!!");
          Category category (STRUCT,"ports");
          return Asset::Ident (name, category );
        }
    } 
  
  /** */
  Port::Port (PProcPatt& wiring, string portID, wstring shortDesc, wstring longDesc) 
    : Struct (createPortIdent (wiring,portID,shortDesc,longDesc)),
      portID_ (portID),
      wiringTemplate(wiring),
      shortDesc (shortDesc),
      longDesc (longDesc)
  {
    
  }

  
  
  PPort 
  Port::query (string properties)
    { 
      return Struct::create (Query<Port> (properties)); 
    }
  
  void 
  Port::switchProcPatt (PProcPatt& another)
  {
    wiringTemplate = another;
    TODO ("trigger rebuild fixture");
  }




} // namespace asset
