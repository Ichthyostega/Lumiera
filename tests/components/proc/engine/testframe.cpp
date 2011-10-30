/*
  TestFrame  -  test data frame (stub) for checking Render engine functionality

  Copyright (C)         Lumiera.org
    2011,               Hermann Vosseler <Ichthyostega@web.de>

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


#include "proc/engine/testframe.hpp"
#include "lib/error.hpp"

#include <cstring>

using std::memcpy;

namespace engine {
namespace test   {
  
  
  namespace error = lumiera::error;
  
  namespace { // hidden local support facilities....
    
  } // (End) hidden impl details
  
  
  TestFrame::~TestFrame() { }  // emit VTables here....
  
  
  TestFrame::TestFrame(uint seq, uint family)
    : discriminator_()
    , stage_(CREATED)
    { }
  
  
  TestFrame::TestFrame (TestFrame const& o)
    : discriminator_(o.discriminator_)
    , stage_(CREATED)
    {
      memcpy (data_, o.data_, BUFFSIZ);
    }
  
  TestFrame&
  TestFrame::operator= (TestFrame const& o)
    {
      if (DISCARDED == stage_)
        throw new error::Logic ("target TestFrame is already dead");
      if (this != &o)
        {
          discriminator_ = o.discriminator_;
          stage_ = CREATED;
          memcpy (data_, o.data_, BUFFSIZ);
        }
      return *this;
    }
  
  
  
  /** whether this output slot is occupied
   * @return true if currently unconnected and
   *         able to connect and handle output data 
   */
  bool
  TestFrame::isAlive (void* memLocation)
  {
    UNIMPLEMENTED ("access memory as TestFrame and check internal accounting");
  }
  
  /** Helper to verify a given memory location holds
   *  an already destroyed TestFrame instance */
  bool
  TestFrame::isDead (void* memLocation)
  {
    UNIMPLEMENTED ("access memory as TestFrame and verify dtor invocation");
  }
  
  bool
  TestFrame::operator== (void* memLocation)  const
  {
    UNIMPLEMENTED ("verify contents of an arbitrary memory location");
  }
  
  bool
  TestFrame::contentEquals (TestFrame const& o)  const
  {
    UNIMPLEMENTED ("equality of test data frames");
  }
  
  bool
  TestFrame::isAlive() const
  {
    UNIMPLEMENTED ("sanity & lifecycle");
  }
  
  bool
  TestFrame::isDead()  const
  {
    UNIMPLEMENTED ("sanity & lifecycle");
  }
  
  bool
  TestFrame::isSane()  const
  {
    UNIMPLEMENTED ("sanity & lifecycle");
  }
  

  
  
  
  TestFrame
  testData (uint seqNr)
  {
    UNIMPLEMENTED ("build, memorise and expose test data frames on demand");
  }
  
  TestFrame
  testData (uint chanNr, uint seqNr)
  {
    UNIMPLEMENTED ("build, memorise and expose test data frames on demand (multi-channel)");
  }

  
  
  
}} // namespace engine::test
