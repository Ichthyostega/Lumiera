/*
  GenericRecordUpdate(Test)  -  manipulate and reshape the generic record contents

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
#include "lib/diff/record-content-mutator.hpp"
//#include "lib/iter-adapter-stl.hpp"
//#include "lib/time/timevalue.hpp"
//#include "lib/format-util.hpp"
#include "lib/util.hpp"

#include <string>
#include <vector>

//using lib::iter_stl::snapshot;
using util::isnil;
//using util::join;
//using std::string;
using std::vector;
//using lib::time::Time;


namespace lib {
namespace diff{
namespace test{
  
  namespace {//Test fixture....
    
  }//(End)Test fixture
  
  
  
  
  
  
  
  
  
  /*****************************************************************************//**
   * @test cover technical details of rearranging the contents of lib::diff::Record.
   *       The implementation of our generic record (abstract object representation)
   *       uses two lists to hold the data of the attribute and content scopes.
   *       When receiving a diff message, we have to rearrange and alter the contents,
   *       which are by default immutable. Thus, for this specific task, embedded
   *       data is moved into this adapter, which exposes the mutating operation
   *       required to apply such a diff message.
   *       
   * @see generic-record-representation-test.cpp
   * @see tree-diff-application-test.cpp
   */
  class GenericRecordUpdate_test
    : public Test
    {
      
      virtual void
      run (Arg)
        {
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (GenericRecordUpdate_test, "unit common");
  
  
  
}}} // namespace lib::diff::test
