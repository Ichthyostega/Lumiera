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
#include <boost/scoped_ptr.hpp>
#include <cstdlib>
//#include <iostream>

//using boost::format;
//using std::string;
//using std::cout;
//using util::for_each;
using boost::scoped_ptr;
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
    
    const size_t SIZE_A = 1 + rand() % TEST_MAX_SIZE;
    const size_t SIZE_B = 1 + rand() % TEST_MAX_SIZE;
    
    const HashVal JUST_SOMETHING = 123;
//  const uint TEST_SIZE = 1024*1024;
//  const uint TEST_ELMS = 20;
      
    
  }
  
  
  /*******************************************************************
   * @test verify the properties of the BufferMetadata records used
   *       internally within BufferProvider to attach additional
   *       organisational data to the exposed buffers.
   */
  class BufferMetadata_test : public Test
    {
      /** common Metadata table to be tested */
      scoped_ptr<Metadata> meta_;
    
      virtual void
      run (Arg) 
        {
          UNIMPLEMENTED ("cover all metadata properties");
          CHECK (ensure_proper_fixture());
          verifyBasicProperties();
          verifyStandardCase();
        }
      
      
      bool
      ensure_proper_fixture() 
      {
        if (!meta_)
          meta_.reset(new Metadata("BufferMetadata_test"));
        
        return (SIZE_A != SIZE_B)
            && (JUST_SOMETHING != meta_->key(SIZE_A))
            && (JUST_SOMETHING != meta_->key(SIZE_B))
            ;
      }
      
      
      void
      verifyBasicProperties()
        {
          HashVal key = meta_->key(SIZE_A);
          CHECK (key);
          
          HashVal key1 = meta_->key(SIZE_A);
          HashVal key2 = meta_->key(SIZE_B);
          CHECK (key1);
          CHECK (key2);
          CHECK (key == key1);
          CHECK (key != key2);
          
          VERIFY_ERROR (INVALID, meta_->get(0))
          VERIFY_ERROR (INVALID, meta_->get(JUST_SOMETHING));
          CHECK ( & meta_->get(key));
          CHECK ( & meta_->get(key1));
          CHECK ( & meta_->get(key2));
          
          CHECK ( isSameObject (meta_->get(key), meta_->get(key)));
          CHECK ( isSameObject (meta_->get(key), meta_->get(key1)));
          CHECK (!isSameObject (meta_->get(key), meta_->get(key2)));
          
          Metadata::Entry& m1 = meta_->get(key);
          CHECK (NIL == m1.state());
          
          VERIFY_ERROR (LIFECYCLE, m1.mark(EMITTED) );
          
          m1.mark (LOCKED);
          CHECK (LOCKED == m1.state());
          CHECK (LOCKED == meta_->get(key1).state());
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
