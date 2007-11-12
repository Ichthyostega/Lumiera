/*
  EFFECT.hpp  -  EDL representation of a pluggable and automatable effect.
 
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


#ifndef MOBJECT_SESSION_EFFECT_H
#define MOBJECT_SESSION_EFFECT_H

#include <string>

#include "proc/mobject/session/abstractmo.hpp"


using std::string;


namespace mobject
  {
  namespace session
    {


    class Effect : public AbstractMO
      {
      protected:
        /** Identifier of the Plugin to be used */
        string plugID;

      };
    
    
     
  } // namespace mobject::session

  ///////////////////////////TODO use macro for specialsation...
  template<>
  class Placement<session::Effect> : public Placement<MObject>
    { 
      Placement (session::Effect & m, void (*moKiller)(MObject*)) 
        : Placement<MObject>::Placement (m, moKiller) 
        { };
        
    public:
      virtual session::Effect*
      operator-> ()  const 
        { 
          ENSURE (INSTANCEOF(session::Effect, &(*this)));
          return static_cast<session::Effect*> (shared_ptr<MObject>::operator-> ()); 
        }      
    };
    
} // namespace mobject
#endif
