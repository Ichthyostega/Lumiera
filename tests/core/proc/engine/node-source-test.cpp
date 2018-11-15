/*
  NodeSource(Test)  -  unit test of the source reading render node

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

/** @file node-source-test.cpp
 ** unit test \ref NodeSource_test
 */


#include "lib/test/run.hpp"
//#include "lib/util.hpp"


//using std::string;


namespace proc {
namespace engine{
namespace test  {
  
  
  
  
  /***************************************************************//**
   * @test the source reading render node.
   */
  class NodeSource_test : public Test
    {
      virtual void run(Arg) 
        {
          UNIMPLEMENTED ("render node pulling source data from vault");
        } 
    };
  
  
  /** Register this test class... */
  LAUNCHER (NodeSource_test, "unit engine");
  
  
  
}}} // namespace proc::engine::test
