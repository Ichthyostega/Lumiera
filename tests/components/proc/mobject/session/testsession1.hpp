/*
  TESTSESSION1.hpp  -  complete session configuration use for various tests
 
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


#ifndef MOBJECT_SESSION_TESTCLIP_H
#define MOBJECT_SESSION_TESTCLIP_H


#include "proc/mobject/session/session.hpp"
#include "common/error.hpp"
//#include "common/factory.hpp"
//#include "common/util.hpp"

//#include <boost/format.hpp>
#include <iostream>

//using boost::format;
using std::string;
using std::cout;


namespace mobject
  {
  namespace session
    {
    typedef std::auto_ptr<Session> PSession;  /////TODO
    
    /**
     * Create a Test Session configuration usable for various Tests.
     * This Session holds two Clips and corresponds to "Example1"
     * in the UML design.
     */
    PSession Testsession1 ()
      {
        UNIMPLEMENTED ("Test-Session 1");
      };
    
    
    
    
  } // namespace session

} // namespace mobject
#endif
