/*
  AddClip(Test)  -  adding an Clip-MObject to the Model/Session

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of
  the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

* *****************************************************/

/** @file add-clip-test.cpp
 ** unit test \ref AddClip_test
 */


#include "lib/test/run.hpp"
#include "proc/mobject/session.hpp"
#include "proc/mobject/session/testclip.hpp"
#include "proc/mobject/placement.hpp"
//#include "lib/format-cout.hpp"
#include "lib/util.hpp"


using util::contains;
//using std::string;


namespace proc {
namespace mobject {
namespace session {
namespace test {
  
  
  
  
  /***************************************************************//**
   * @test adding an test clip to the Model/Session.
   * @see  mobject::session::Clip
   * @see  mobject::Session
   */
  class AddClip_test : public Test
    {
      virtual void
      run (Arg) 
        {
          PSess sess = Session::current;
          PMO clip = TestClip::create();
          sess->attach (clip);
          
///////////////////////////////////////////////////////////////////TICKET #499              
//            CHECK (sess->currEDL().contains (clip));
          // TODO: Clip-Asset and Placement magic??
        } 
    };
  
  
  /** Register this test class... */
  LAUNCHER (AddClip_test, "unit session");
  
  
  
}}}} // namespace proc::mobject::session::test
