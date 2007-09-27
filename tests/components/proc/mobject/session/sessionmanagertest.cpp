/*
  SessionManager(Test)  -  accessing, loading and saving the Session/EDL
 
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


#include "common/test/run.hpp"
#include "proc/mobject/session.hpp"
#include "proc/mobject/session/testsession1.hpp"
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
    namespace test
      {
      
      
      /*******************************************************************************
       * Check the session management operations provided by mobject::session::Seesion
       * This includes accessing the current Session (somewhat a singleton).
       * @todo load a Mock session
       * @todo create a session and save (serialize) it
       * @todo load a real test session
       */
      class SessionManager_test : public Test
        {
          virtual void run(Arg arg) 
            {
              getCurrentSession ();
              clearSession();
              loadMockSession();
              
              clearSession();
              buildTestseesion1();
              string serialized;
              saveSession (serialized);
              loadSession (serialized);
              ASSERT (checkTestsession1());
            } 
          
          /** @test accessing the current (global) session 
           */
          void getCurrentSession ()
            {
              PSess sess = Session::current;
              ASSERT (sess->isValid());
            }
          
          /** @test clear current session contents 
           *        without resetting global session config.
           * @todo  implement all session content, implement 
           *        mobject and asset deletion operations.
           */
          void clearSession ()
            {
              UNIMPLEMENTED ("clear objects in current session");
              Session::current.clear();
            }
          
          /** @test reset global session config and start with
           *        a pristine default session. 
           * @todo  define the "global session config", implement session default ctor
           */
          void resetSession ()
            {
              UNIMPLEMENTED ("construct a pristine session");
              Session::current.reset();
            }
          
          /** @test use a mock session serializer to load 
           *        a preconfigured test session. Verify
           *        objects are wired correctly.
           *  @todo implement rebuilding session, implement mock session serializer 
           */
          void loadMockSession ()
            {
              UNIMPLEMENTED ("rebuild session using a mock serializer");
            }
          
          /** @test load serialized session using the
           *        real session serializer implementation.
           * @param src string with serialized session data
           * @todo  implement real session serializer 
           */
          void loadSession (const string& src)
            {
              UNIMPLEMENTED ("loding real sesion");
            }
          
          /** @test serialize (save) the current session
           * @param dest string recieving the generated serialized stream
           * @todo  implement real session serializer
           */
          void saveSession (string& dest)
            {
              UNIMPLEMENTED ("serialize current session");
            }
        };
      
      
      /** Register this test class... */
      LAUNCHER (SessionManager_test, "function session");
      
      
      
    } // namespace test
  
  } // namespace session

} // namespace mobject
