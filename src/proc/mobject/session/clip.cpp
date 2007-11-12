/*
  Clip  -  a Media Clip
 
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


#include "proc/mobject/session/clip.hpp"

namespace mobject
  {
  namespace session
    {

    /** new clip-MO linked with the given asset::Clip.
     *  Initially, this clip will cover the whole source media length. 
     */
    Clip::Clip (PClipAsset& mediaDef)
      : start_(0),
        mediaDef_(mediaDef)
    {
      setupLength();
    }
    
    
    
    /** implementing the common MObject self test.
     *  Length definition is consitent, underlying 
     *  media def is accessible etc. */
    bool
    Clip::isValid ()  const
    {
      UNIMPLEMENTED ("check consistency of clip length def, implies accessing the underlying media def");
    }
    
    
    void 
    Clip::setupLength()
    {
      UNIMPLEMENTED ("calculate the length of a clip and set length field");
      // will use mediaDef to query media parameters....
    }





  } // namespace mobject::session

} // namespace mobject
