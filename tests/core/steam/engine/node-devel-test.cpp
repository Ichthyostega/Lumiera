/*
  NodeDevel(Test)  -  verify proper render node operation and calldown

  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file node-devel-test.cpp
 ** unit test \ref NodeDevel_test
 */


#include "lib/test/run.hpp"
//#include "lib/util.hpp"


//using std::string;


namespace steam {
namespace engine{
namespace test  {
  
  
  
  
  /***************************************************************//**
   * @test check render node operation modes and collaboration.
   */
  class NodeDevel_test : public Test
    {
      virtual void run(Arg) 
        {
          UNIMPLEMENTED ("operate some render nodes as linked together");
        } 
    };
  
  
  /** Register this test class... */
  LAUNCHER (NodeDevel_test, "unit node");
  
  
  
}}} // namespace steam::engine::test
