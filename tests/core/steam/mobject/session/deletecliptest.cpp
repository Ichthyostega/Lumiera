/*
  DeleteClip(Test)  -  removing an Clip-MObject from the Session

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

/** @file deletecliptest.cpp
 ** unit test \ref DeleteClip_test
 */


#include "lib/test/run.hpp"
#include "steam/assetmanager.hpp"
#include "steam/mobject/session.hpp"
#include "steam/mobject/session/testsession1.hpp"
#include "steam/mobject/session/clip.hpp"              // TODO: really neded?
#include "lib/format-cout.hpp"
//#include "lib/util.hpp"


//using std::string;

using proc_interface::AssetManager;
using proc_interface::PAsset;
using proc_interface::IDA;

using std::dynamic_pointer_cast;        /// TODO only temp


namespace steam    {
namespace mobject {
namespace session {
namespace test    {
  
  
  
  
  /***************************************************************//**
   * @test removing a test clip from the Session/Model.
   * @see  mobject::session::Clip
   * @see  mobject::Session
   * 
   * @todo ////////////////////////////////////////////////TICKET #499
   */
  class DeleteClip_test : public Test
    {
      virtual void
      run (Arg) 
        {
          buildTestsession1();
          PSess sess = Session::current;
          AssetManager& aMang = AssetManager::instance();
          
          UNIMPLEMENTED("typesafe searching for MObjects in the Session");
          
          
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #639 : work out how to search within the Model!!
                
          PClipMO clipPlacement = sess->currEDL().find(SESSION1_CLIP); 
                                                              // global Var assigned in buildTestsession1()
          PMedia media = clipPlacement->getMedia();
          IDA clipAID = media->getID();
          CHECK (clipPlacement);
          
          sess->remove (clipPlacement);
          
          CHECK (!sess->currEDL().find(SESSION1_CLIP));            // Session forgot the Clip/Placement
          CHECK (!aMang.known (clipAID));                          // corresponding Clip Asset has disappeared
          CHECK (!clipPlacement->getMedia());                      // internal cross-links removed
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #639 : work out how to search within the Model!!
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (DeleteClip_test, "function session");
  
  
  
}}}} // namespace steam::mobject::session::test
