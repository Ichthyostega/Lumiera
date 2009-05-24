/*
  PlacementHierarchy(Test)  -  concept draft how to deal with the MObject hierarchy in Placements
 
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
//#include "proc/mobject/session/testclip.hpp"
//#include "proc/mobject/placement.hpp"
//#include "proc/mobject/placement-ref.hpp"
//#include "proc/mobject/explicitplacement.hpp"
//#include "lib/util.hpp"

#include <boost/format.hpp>
#include <tr1/memory>
#include <iostream>

using std::tr1::shared_ptr;
using boost::format;
//using lumiera::Time;
//using util::contains;
using std::string;
using std::cout;


namespace mobject {
namespace session {
namespace test    {
  
  
  
  template<class TY, class B>
  struct Pla;
  
  struct MOj { };
  struct Sub1 : MOj { };
  
  template<>
  struct Pla<MOj,MOj>
    :  shared_ptr<MOj>
    {
      
    };
  
  template<class TY, class B>
  struct Pla
    : Pla<MOj, MOj>
    {
      
    };
  
  
  /*******************************************************************************
   * @test Currently (5/09) this is a concept draft separate of any existing types.
   *       The intention is to rework the Placement implementation based on the
   *       outcomes of this experiment. When doing so, this test could later on
   *       serve to document and cover the corresponding Placement properties.
   * @see  mobject::Placement
   * @see  mobject::MObject#create
   */
  class PlacementHierarchy_test : public Test
    {
      
      virtual void
      run (Arg) 
        {
          Pla<Sub1,MOj> pSub1;
          
          /////////////////////////////////TODO
          format fmt ("sizeof( %s ) = %d\n");
          cout << fmt % "Pla<Sub1>"     % sizeof(pSub1);
          cout << "Hurgha!\n";
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (PlacementHierarchy_test, "unit session");
  
  
}}} // namespace mobject::session::test
