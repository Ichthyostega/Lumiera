/*
  AddClip(Test)  -  adding an Clip-MObject to the EDL/Session
 
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
#include "proc/mobject/session/testclip.hpp"
#include "proc/mobject/placement.hpp"
#include "common/util.hpp"

//#include <boost/format.hpp>
#include <iostream>

//using boost::format;
using cinelerra::Time;
using util::contains;
using std::string;
using std::cout;


namespace mobject
  {
  namespace session
    {
    namespace test
      {
      
      
      
      
      /*******************************************************************
       * @test adding an test clip to the EDL/Session.
       * @see  mobject::session::Clip
       * @see  mobject::session::EDL
       */
      class AddClip_test : public Test
        {
          virtual void run(Arg arg) 
            {
              Session& sess = Session::getCurrent();
              PMO clip = TestClip::create();
              PPla pla = Placement::create(Placement::FIXED, Time(1), clip);
              sess.add (pla);
              
              ASSERT (contains (sess.getEDL(), pla));
              // TODO: Clip-Asset and Placement magic??
            } 
        };
      
      
      /** Register this test class... */
      LAUNCHER (AddClip_test, "unit session");
      
      
      
    } // namespace test
  
  } // namespace session

} // namespace mobject
