/*
  SESSION.hpp  -  holds the complete session to be edited by the user
 
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


#ifndef MOBJECT_SESSION_SESSION_H
#define MOBJECT_SESSION_SESSION_H

#include "common/singleton.hpp"
#include "proc/mobject/placement.hpp"

#include <boost/utility.hpp>



namespace mobject
  {
  namespace session
    {

    class EDL;
    class Fixture;

    /**
     * The (current) Session holds all the user
     * visible content to be edited and manipulated
     * within the Cinelerra Application. From a users
     * perspective, it is a collection of Media Objects
     * (--> MObject) placed (--> Placement) onto virtual
     * Tracks.
     * 
     * Opening a Session has effectively global consequences, 
     * because the Session defines the available Assets, and some 
     * kinds of Assets define default behaviour. Thus, access to 
     * the Session is similar to a Singleton instance.
     *  
     */
    class Session : private boost::noncopyable
      {
      protected:
        vector<EDL> edls;
        Fixture fixture;
        
        Session ();
        friend class cinelerra::singleton::StaticCreate<SessionImpl>; //TODO use PImpl or just covariance??

      public:
        static cinelerra::Singleton<Session> getCurrent;
        
        void add (PPla placement);
        
        
        EDL& currEDL () { return edl; }
        Fixture& getFixture () { return fixture; }

      };



  } // namespace mobject::session

} // namespace mobject
#endif
