/*
  MObjectFactory  -  creating concrete MObject subclass instances
 
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


#include "proc/mobject/session/mobjectfactory.hpp"
#include "proc/mobject/session/clip.hpp"
#include "proc/mobject/session/effect.hpp"
#include "proc/asset/clip.hpp"
#include "proc/asset/effect.hpp"

namespace mobject
  {
  namespace session
    {

    /** creating a Clip-MObject to be placed within
     *  the EDL, based on a clip asset, which typically
     *  is obtained by calling the createClip()-function
     *  on some asset::Media. The newly created Clip-MO is
     *  internally linked with this media Asset and wrapped
     *  into a Placement, which takes ownership. So, when the
     *  render engine gets across this Clip-MO, it is able to
     *  obtain the media information contained in the corresponding
     *  media asset. Depending on the mediaDef passed in, the 
     *  created Clip could be a compound (multichannel) clip 
     *  comprised of several SimpleClip subobjects.
     *  @todo implement creation of multichannel CompoundClip
     */
    Placement<Clip>   
    MObjectFactory::operator() (PClipAsset& mediaDef)
    {
      return Placement<Clip> (new Clip (mediaDef));
    }
    
    
    /** */
    Placement<Effect>
    MObjectFactory::operator() (asset::Effect processorDef)
    {
      
    }



  } // namespace mobject::session

} // namespace mobject
