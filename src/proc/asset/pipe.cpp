/*
  Pipe  -  structural asset denoting a processing pipe to generate media output
 
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


#include "proc/asset/pipe.hpp"
#include "lib/util.hpp"

using util::isnil;

namespace asset {
  
  
  /** Create and register a new Pipe asset.
   *  Usually, this is triggered automatically
   *  by referring to the pipeID. When building
   *  the render network, the given processing pattern
   *  will be executed, allowing for all sorts of
   *  default wiring.
   */
  Pipe::Pipe ( const Asset::Ident& idi
             , PProcPatt& wiring
             , string shortName
             , string longName
             ) 
    : Struct (idi)
    , wiringTemplate(wiring)
    , shortDesc (shortName)
    , longDesc (longName)
  {
    REQUIRE (idi.isValid());
    if (isnil (shortDesc))
      shortDesc = string(idi);
  }

  
  
  PPipe 
  Pipe::query (string properties)
    { 
      return Struct::retrieve (Query<Pipe> (properties)); 
    }
  
  void 
  Pipe::switchProcPatt (PProcPatt& another)
  {
    wiringTemplate = another;
    TODO ("trigger rebuild fixture");
  }




} // namespace asset
