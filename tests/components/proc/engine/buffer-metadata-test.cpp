/*
  BufferMetadata(Test)  -  properties of internal data buffer metadata

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


#include "lib/error.hpp"
#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
//#include "lib/util-foreach.hpp"
#include "lib/util.hpp"
//#include "proc/play/diagnostic-output-slot.hpp"
//#include "proc/engine/testframe.hpp"
//#include "proc/engine/diagnostic-buffer-provider.hpp"
#include "proc/engine/buffer-metadata.hpp"
//#include "proc/engine/buffhandle.hpp"
//#include "proc/engine/bufftable.hpp"

//#include <boost/format.hpp>
#include <cstdlib>
//#include <iostream>

//using boost::format;
//using std::string;
//using std::cout;
//using util::for_each;
using util::isnil;
using util::isSameObject;


namespace engine{
namespace test  {
  
//  using lib::AllocationCluster;
//  using mobject::session::PEffect;
//  using ::engine::BuffHandle;
  using lumiera::error::LUMIERA_ERROR_INVALID;
  using lumiera::error::LUMIERA_ERROR_LIFECYCLE;
  
  
  namespace { // Test fixture
    
    const size_t TEST_MAX_SIZE = 1024 * 1024;
    
    const size_t SIZE_A = 1 + rand() % TEST_MAX_SIZE 
    const size_t SIZE_B = 1 + rand() % TEST_MAX_SIZE 
    
    const HashVal JUST_SOMETHING = 123;
//  const uint TEST_SIZE = 1024*1024;
//  const uint TEST_ELMS = 20;
      
    bool
    ensure_proper_fixture() 
    {
      return (SIZE_A != SIZE_B)
          && (JUST_SOMETHING != Metadata::key(SIZE_A))
          && (JUST_SOMETHING != Metadata::key(SIZE_B))
          ;
    }
    
  }
  
  
  /*******************************************************************
   * @test verify the properties of the BufferMetadata records used
   *       internally within BufferProvider to attach additional
   *       organisational data to the exposed buffers.
   */
  class BufferMetadata_test : public Test
    {
      virtual void
      run (Arg) 
        {
          UNIMPLEMENTED ("cover all metadata properties");
          CHECK (ensure_proper_fixture());
          verifyBasicProperties();
          verifyStandardCase();
        }
      
      
      void
      verifyBasicProperties()
        {
          HashVal key = Metadata::key(SIZE_A);
          CHECK (key);
          
          HashVal key1 = Metadata::key(SIZE_A);
          HashVal key2 = Metadata::key(SIZE_B);
          CHECK (key1);
          CHECK (key2);
          CHECK (key == key1);
          CHECK (key != key2);
          
          VERIFY_ERROR (INVALID, Metadata::get(0))
          VERIFY_ERROR (INVALID, Metadata::get(JUST_SOMETHING));
          CHECK ( & Metadata::get(key));
          CHECK ( & Metadata::get(key1));
          CHECK ( & Metadata::get(key2));
          
          CHECK ( isSameObject (Metadata::get(key), Metadata::get(key)));
          CHECK ( isSameObject (Metadata::get(key), Metadata::get(key1)));
          CHECK (!isSameObject (Metadata::get(key), Metadata::get(key2)));
          
          Metadata& m1 = Metadata::get(key);
          CHECK (NIL == m1.state());
          
          VERIFY_ERROR (LIFECYCLE, m1.mark(EMITTED) );
          
          m1.mark (LOCKED);
          CHECK (LOCKED == m1.state());
          CHECK (LOCKED == Metadata::get(key1).state());
        }
      
      
      void
      verifyStandardCase()
        {
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #834
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #834
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (BufferMetadata_test, "unit player");
  
  
  
}} // namespace engine::test
