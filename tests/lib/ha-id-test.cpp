/*
  HaID(Test)  -  proof-of-concept test for a hash based and typed ID
 
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
#include "lib/ha-id.hpp"
//#include "proc/mobject/explicitplacement.hpp"
//#include "lib/util.hpp"

//#include <boost/format.hpp>
//#include <iostream>

//using boost::format;
//using lumiera::Time;
//using util::contains;
using std::string;
//using std::cout;


namespace lib {
namespace test{
      
  /** @todo WIP a generic hash-index, maybe also usable for assets */
  HaID<TestBB,TestA> hahaBB1;
  
  TestBA bab;
  HaID<TestBA,TestA> hahaBA1 (bab);
  ///////////////////////////////TODO (Experimentation)
  
  
  /***************************************************************************
   * @test proof-of-concept test for a generic hash based and typed ID struct.
   * @see  lib::HaID
   */
  class HaID_test : public Test
    {
      
      virtual void
      run (Arg) 
        {
          /////////////////////////////////TODO
        } 
    };
  
  
  /** Register this test class... */
  LAUNCHER (HaID_test, "unit common");
      
      
}} // namespace lib::test
