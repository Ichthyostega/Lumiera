/*
  DeleteClip(Test)  -  removing an Clip-MObject from the Session
 
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
#include "proc/assetmanager.hpp"
#include "proc/mobject/session.hpp"
#include "proc/mobject/session/edl.hpp"               // TODO: really neded?
#include "proc/mobject/session/testsession1.hpp"
#include "proc/mobject/session/clip.hpp"              // TODO: really neded?
//#include "lib/util.hpp"

//#include <boost/format.hpp>
#include <iostream>

//using boost::format;
using std::string;
using std::cout;

using proc_interface::AssetManager;
using proc_interface::PAsset;
using proc_interface::IDA;

using std::tr1::dynamic_pointer_cast;        /// TODO only temp


namespace mobject {
namespace session {
namespace test    {
  
  
  
  
  /*******************************************************************
   * @test removing a test clip from the EDL.
   * @see  mobject::session::Clip
   * @see  mobject::session::EDL
   */
  class DeleteClip_test : public Test
    {
      virtual void
      run (Arg) 
        {
          buildTestsession1();
          PSess sess = Session::current;
          AssetManager& aMang = AssetManager::instance();
          
          UNIMPLEMENTED("typesafe searching for MObjects in the EDL");
          
          
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////TODO: work out how to search within EDL!!
                
          PClipMO clipPlacement = sess->currEDL().find(SESSION1_CLIP); 
                                                              // global Var asigned in buildTestsession1()
          PMedia media = clipPlacement->getMedia();
          IDA clipAID = media->getID();
          ASSERT (clipPlacement);
          
          sess->remove (clipPlacement);
          
          ASSERT (!sess->currEDL().find(SESSION1_CLIP));            // EDL forgot the Clip/Placement
          ASSERT (!aMang.known (clipAID));                          // corresponding Clip Asset has disappeared
          ASSERT (!clipPlacement->getMedia());                      // internal cross-links removed
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////TODO: work out how to search within EDL!!
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (DeleteClip_test, "function session");
  
  
  
}}} // namespace mobject::session::test
