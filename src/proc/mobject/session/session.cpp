/*
  Session  -  holds the complete session to be edited by the user
 
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


#include "proc/mobject/session/session.hpp"
#include "proc/mobject/session/edl.hpp"
#include "proc/mobject/session/fixture.hpp"


using cinelerra::Singleton;

namespace mobject
  {
  namespace session
    {

    /** @return the system-wide current session.
     *  Implemented as singleton.
     */
    Singleton<Session> Session::getCurrent;
    
    
    
    /** create a new empty session with default values.
     */
    Session::Session ()
      : edl(), 
        fixture() 
      {
        
      }



  } // namespace mobject::session

} // namespace mobject
