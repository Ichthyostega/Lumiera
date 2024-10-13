/*
  TEST-RAND-ONTOLOGY.hpp  -  placeholder for a domain-ontology working on dummy data frames

  Copyright (C)         Lumiera.org
    2024,               Hermann Vosseler <Ichthyostega@web.de>

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

*/

/** @file testframe-rand-ontology.hpp
 ** A faked »media calculation« environment to validate the render node network.
 */


#ifndef STEAM_ENGINE_TEST_RAND_ONTOLOGY_H
#define STEAM_ENGINE_TEST_RAND_ONTOLOGY_H


#include "steam/engine/testframe.hpp"

#include <array>


namespace steam {
namespace engine{
namespace test  {
  

/////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1367 : Dummy / Placeholder
  using SoloArg = std::array<char*, 1>;
  /** @todo a placeholder operation to wire a prototypical render node
   */
  inline void
  dummyOp (SoloArg in, SoloArg out)
  {
    UNIMPLEMENTED ("a sincerely nonsensical operation");
  }
/////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1367 : Dummy / Placeholder
  /**
   * A fake _Domain Ontology_ to describe mocked »render operations« on
   * dummy data frames filled with random numbers.
   * 
   * @see TestFrame_test
   * @see NodeDevel_test
   * @see NodeLinkage_test
   * 
   */
  class TestRandOntology
    {
      
    public:
     ~TestRandOntology()  = default;
      TestRandOntology()  = default;
      
    private:
    };
  
  
  
  /** */
  
  
  
}}} // namespace steam::engine::test
#endif
