/*
  MOBJECTFACTORY.hpp  -  creating concrete MObject subclass instances
 
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
 
*/


#ifndef MOBJECT_SESSION_MOBJECTFACTORY_H
#define MOBJECT_SESSION_MOBJECTFACTORY_H

#include "proc/mobject/mobject.hpp"



namespace asset
  {
  class Clip;
  class Effect;
  
  }

namespace mobject
  {
  namespace session
    {
    class Clip;
    class Effect;

    typedef shared_ptr<const asset::Clip> PClipAsset;

    class MObjectFactory
      {
        /** custom deleter func allowing class Placement 
          *  to take ownership of MObjct instances
          */
        static void deleterFunc (MObject* o) { delete o; }
        
      public:
        
        Placement<Clip>   operator() (PClipAsset&);
        Placement<Effect> operator() (const asset::Effect);
        
      };



  } // namespace mobject::session

} // namespace mobject
#endif
