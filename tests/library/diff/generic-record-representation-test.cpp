/*
  GenericRecordRepresentation(Test)  -  introspective representation of object-like data

  Copyright (C)         Lumiera.org
    2015,               Hermann Vosseler <Ichthyostega@web.de>

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


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/diff/record.hpp"

//#include <utility>
//#include <string>
//#include <vector>

//using std::string;
//using std::vector;
//using std::swap;


namespace lib {
namespace diff{
namespace test{
  
//  using lumiera::error::LUMIERA_ERROR_LOGIC;
  
  namespace {//Test fixture....
    
    
    
  }//(End)Test fixture
  
  
  
  
  
  
  
  
  
  /*****************************************************************************//**
   * @test Verify properties of a special collection type meant for external representation
   *       of object-loke data.
   *       
   * @see IndexTable
   * @see DiffListApplication_test
   */
  class GenericRecordRepresentation_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          simpleUsage();
          verifySnapshot();
          sequenceIteration();
          duplicateDetection();
          copy_and_move();
        }
      
      
      void
      simpleUsage()
        {
        }
      
      
      void
      verifySnapshot()
        {
        }
      
      
      void
      sequenceIteration()
        {
        }
      
      
      void
      duplicateDetection()
        {
        }
      
      
      void
      copy_and_move()
        {
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (GenericRecordRepresentation_test, "unit common");
  
  
  
}}} // namespace lib::diff::test
