/*
  SessionServiceAccess(Test)  -  accessing implementation level session services
 
  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>
 
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


#include "lib/test/run.hpp"
#include "proc/mobject/session.hpp"
//#include "proc/mobject/session/testsession1.hpp"
#include "proc/mobject/session/session-services.hpp"
//#include "lib/util.hpp"
//#include <boost/format.hpp>
//#include <iostream>

//using boost::format;
//using std::string;
//using std::cout;


namespace mobject {
namespace session {
namespace test    {
  
  
  /*******************************************************************************
   * Verify the access mechanism used by Proc-Layer internals for 
   * accessing implementation level APIs of the session.
   * 
   * @todo WIP-WIP
   */
  class SessionServiceAccess_test : public Test
    {
      virtual void
      run (Arg arg) 
        {
//          getCurrentSession ();
//          clearSession();
//          loadMockSession();
//          
//          clearSession();
//          buildTestsession1();
//          string serialized;
//          saveSession (serialized);
//          loadSession (serialized);
//          ASSERT (checkTestsession1());
        } 
      
      
      /** @test accessing the current (global) session */
      void
      getCurrentSession ()
        {
//          PSess sess = Session::current;
//          ASSERT (sess->isValid());
        }
      
      
    };
  
  
  /** Register this test class... */
  LAUNCHER (SessionServiceAccess_test, "function session");
  
  
  
}}} // namespace mobject::session::test
