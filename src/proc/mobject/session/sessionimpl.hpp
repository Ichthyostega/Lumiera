/*
  SESSIONIMPL.hpp  -  holds the complete session data to be edited by the user
 
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


#ifndef MOBJECT_SESSION_SESSIONIMPL_H
#define MOBJECT_SESSION_SESSIONIMPL_H

#include "proc/mobject/session/session.hpp"
#include "proc/mobject/session/edl.hpp"
#include "proc/mobject/session/fixture.hpp"

#include <vector>

using std::vector;



namespace mobject
  {
  namespace session
    {


    /**
     * Implementation class for the Session interface
     */
    class SessionImpl : public mobject::Session
      {
      protected:
        vector<EDL> edls;
        Fixture fixture;
        
        SessionImpl ();
        friend class cinelerra::singleton::StaticCreate<SessionImpl>;
        
        void add (PPla placement);
        
        EDL& currEDL () { return edl; }
        Fixture& getFixture () { return fixture; }

      };

  } // namespace mobject::session

} // namespace mobject
#endif
