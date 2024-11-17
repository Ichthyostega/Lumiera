/*
  SessionManager(Test)  -  accessing, loading and saving the Session/Model

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file session-manager-test.cpp
 ** unit test \ref SessionManager_test
 */


#include "lib/test/run.hpp"
#include "steam/mobject/session.hpp"
#include "steam/mobject/session/testsession1.hpp"
//#include "lib/util.hpp"

//using std::string;


namespace steam    {
namespace mobject {
namespace session {
namespace test    {
  
  
  /***************************************************************************//**
   * Check the session management operations provided by mobject::session::Session
   * This includes accessing the current Session (somewhat a singleton).
   * @todo load a Mock session
   * @todo create a session and save (serialise) it
   * @todo load a real test session
   */
  class SessionManager_test : public Test
    {
      virtual void
      run (Arg arg) 
        {
          getCurrentSession ();
          clearSession();
          loadMockSession();
          
          clearSession();
          buildTestsession1();
          string serialized;
          saveSession (serialized);
          loadSession (serialized);
          CHECK (checkTestsession1());
        } 
      
      
      /** @test accessing the current (global) session */
      void
      getCurrentSession ()
        {
          PSess sess = Session::current;
          CHECK (sess->isValid());
        }
      
      
      /** @test clear current session contents 
       *        without resetting global session config.
       * @todo  implement all session content, implement
       *        mobject and asset deletion operations.
       */
      void
      clearSession ()
        {
          UNIMPLEMENTED ("clear objects in current session");
          Session::current.clear();
        }
      
      
      /** @test reset global session config and start with
       *        a pristine default session. 
       * @todo  define the "global session config", implement session default ctor
       */
      void
      resetSession ()
        {
          UNIMPLEMENTED ("construct a pristine session");
          Session::current.reset();
        }
      
      
      /** @test use a mock session serialiser to load 
       *        a preconfigured test session. Verify
       *        objects are wired correctly.
       *  @todo implement rebuilding session, implement mock session serialiser
       */
      void
      loadMockSession ()
        {
          UNIMPLEMENTED ("rebuild session using a mock serialiser");
        }
      
      
      /** @test load serialised session using the
       *        real session serialiser implementation.
       * @param src string with serialised session data
       * @todo  implement real session serialiser 
       */
      void
      loadSession (const string& src)
        {
          UNIMPLEMENTED ("loading real session");
        }
      
      
      /** @test serialise (save) the current session
       * @param dest string receiving the generated serialised stream
       * @todo  implement real session serialiser
       */
      void
      saveSession (string& dest)
        {
          UNIMPLEMENTED ("Serialise current session");
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (SessionManager_test, "function session");
  
  
  
}}}} // namespace steam::mobject::session::test
