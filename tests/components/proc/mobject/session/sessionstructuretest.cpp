/*
  SessionStructure(Test)  -  verifying basic Session/EDL structure
 
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
#include "proc/mobject/session/session.hpp"
#include "proc/assetmanager.hpp"
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
      using proc_interface::AssetManager;
      using proc_interface::PAsset;
      
      
      /*******************************************************************************
       * @test access the current session and verify the correct
       *       structure of the most important components: The session
       *       contains an EDL, we can get at the Fixture, we have at least
       *       one Track and the corresponding Track asset is available.
       * @todo define further criteria to be checked 
       * @todo implement EDL, Fixture, Session#rebuildFixture, asset::Track
       */
      class SessionStructure_test : public Test
        {
          virtual void run(Arg arg) 
            {
              Session& sess = Session::getCurrent();
              ASSERT (0 <= sess.getEDL().size());       // TODO implement
              ASSERT (0 <= sess.getFixture().size());   // TODO implement
              ASSERT (0 <  sess.getTracks().size());    // TODO implement
              
              PAsset track = sess.getTracks()[0];
              AssetManager& aMang = AssetManager::instance();
              ASSERT (track == aMang.getAsset (track->getID()));
            } 
        };
      
      
      /** Register this test class... */
      LAUNCHER (SessionStructure_test, "unit session");
      
      
      
    } // namespace test
  
  } // namespace session

} // namespace mobject
