/*
  PlacementIndex(Test)  -  facility keeping track of Placements within the Session
 
  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>
 
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
//#include "proc/asset/media.hpp"
//#include "proc/mobject/session.hpp"
//#include "proc/mobject/session/edl.hpp"
#include "proc/mobject/session/placement-index.hpp"
#include "proc/mobject/session/testclip.hpp"
#include "proc/mobject/placement.hpp"
//#include "proc/mobject/explicitplacement.hpp"
//#include "lib/util.hpp"

//#include <boost/format.hpp>
//#include <iostream>

//using boost::format;
//using lumiera::Time;
//using util::contains;
using std::string;
//using std::cout;


namespace mobject {
namespace session {
namespace test    {
      
  using asset::VIDEO;
  using session::test::TestClip;
  
  
  
  /***************************************************************************
   * @test basic behaviour of the index mechanism used to keep track of
   *       individual Placements as added to the current Session..
   * @see  mobject::Placement
   * @see  mobject::MObject#create
   * @see  mobject::Placement#addPlacement
   * @see  mobject::Placement#resolve
   */
  class PlacementIndex_test : public Test
    {
      typedef shared_ptr<PlacementIndex> PIdx;
      
      virtual void
      run (Arg) 
        {
          PIdx index (PlacementIndex::create());
          ASSERT (index);
          
          /////////////////////////////////TODO
          checkSimpleInsert (index);
        }
      
      void
      checkSimpleInsert (PIdx index)
        {
          PMO clip = TestClip::create();
          PMO& root = index->getRoot();
          
          ASSERT (0 == index->size());
          ASSERT (!index->contains (clip));
          
//        index->insert (clip, root);
          ASSERT (1 == index->size());
          ASSERT ( index->contains (clip));
          
//        index->remove(clip);
          ASSERT (0 == index->size());
          ASSERT (!index->contains (clip));
          ASSERT ( index->contains (root));
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (PlacementIndex_test, "unit session");
      
      
}}} // namespace mobject::session::test
