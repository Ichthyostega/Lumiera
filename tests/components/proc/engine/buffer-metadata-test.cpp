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
#include "proc/engine/testframe.hpp"
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
    const void* const  SOME_POINTER = &JUST_SOMETHING;
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
      scoped_ptr<BufferMetadata> meta_;
      
      virtual void
      run (Arg) 
        {
          CHECK (ensure_proper_fixture());
          verifyBasicProperties();
          verifyStandardCase();
          UNIMPLEMENTED ("cover all metadata properties");
        }
      
      
      bool
      ensure_proper_fixture() 
        {
          if (!meta_)
            meta_.reset(new BufferMetadata("BufferMetadata_test"));
          
          return (SIZE_A != SIZE_B)
              && (JUST_SOMETHING != meta_->key(SIZE_A))
              && (JUST_SOMETHING != meta_->key(SIZE_B))
              ;
        }
      
      
      void
      verifyBasicProperties()
        {
          // retrieve some type keys
          metadata::Key key = meta_->key(SIZE_A);
          CHECK (key);
          
          metadata::Key key1 = meta_->key(SIZE_A);
          metadata::Key key2 = meta_->key(SIZE_B);
          CHECK (key1);
          CHECK (key2);
          CHECK (key == key1);
          CHECK (key != key2);
          
          // access metadata entries
          VERIFY_ERROR (INVALID, meta_->get(0));
          VERIFY_ERROR (INVALID, meta_->get(JUST_SOMETHING));
          CHECK ( & meta_->get(key));
          CHECK ( & meta_->get(key1));
          CHECK ( & meta_->get(key2));
          
          CHECK ( isSameObject (meta_->get(key), meta_->get(key)));
          CHECK ( isSameObject (meta_->get(key), meta_->get(key1)));
          CHECK (!isSameObject (meta_->get(key), meta_->get(key2)));
          
          // entries retrieved thus far were inactive (type only) entries
          metadata::Entry& m1 = meta_->get(key);
          CHECK (NIL == m1.state());
          CHECK (!meta_->isLocked(key));
          
          VERIFY_ERROR (LIFECYCLE, m1.mark(EMITTED) );
          VERIFY_ERROR (LIFECYCLE, m1.mark(LOCKED)  );
          
          // now create an active (buffer) entry
          metadata::Entry& m2 = meta_->markLocked (key, SOME_POINTER);
          CHECK (!isSameObject (m1,m2));
          CHECK (NIL    == m1.state());
          CHECK (LOCKED == m2.state());
          CHECK (SOME_POINTER == m2.access()); // buffer pointer associated
          
          // entries are unique and identifiable
          HashVal keyX = meta_->key(key1, SOME_POINTER);
          CHECK (meta_->isLocked(keyX));
          CHECK (keyX != key1);
          CHECK (keyX);
          
          CHECK ( isSameObject (m1, meta_->get(key)));
          CHECK ( isSameObject (m1, meta_->get(key1))); 
          CHECK ( isSameObject (m2, meta_->get(keyX)));
          CHECK ( key1 == m2.parentKey());
          
          // now able to do state transitions
          CHECK (LOCKED == m2.state());
          m2.mark(EMITTED);
          CHECK (EMITTED == m2.state());
          CHECK (SOME_POINTER == m2.access());
          CHECK ( meta_->isLocked(keyX));
          CHECK ( meta_->isKnown(keyX));
          
          // but the FREE state is a dead end
          m2.mark(FREE); 
          CHECK (!meta_->isLocked(keyX));
          CHECK ( meta_->isKnown(keyX));
          CHECK ( meta_->isKnown(key1));
          VERIFY_ERROR (LIFECYCLE, m2.access());
          VERIFY_ERROR (LIFECYCLE, m2.mark(LOCKED));
          CHECK ( isSameObject (m2, meta_->get(keyX))); // still accessible
          
          // release buffer...
          meta_->release(keyX);
          CHECK (!meta_->isLocked(keyX));
          CHECK (!meta_->isKnown(keyX));
          CHECK ( meta_->isKnown(key1));
          VERIFY_ERROR (INVALID, meta_->get(keyX)); // now unaccessible
        }
      
      
      /** @test simulate a standard buffer provider usage cycle 
       *  @note to get the big picture, please refer to
       *        BufferProviderProtocol_test#verifyStandardCase()
       *        This testcase here performs precisely the metadata related
       *        operations necessary to carry out the standard case outlined
       *        in that more high level test.
       */
      void
      verifyStandardCase()
        {
          // to build a descriptor for a buffer holding a TestFrame
          TypeHandler attachTestFrame = TypeHandler::create<TestFrame>();
          metadata::Key bufferType1 = meta_->key(sizeof(TestFrame), attachTestFrame);
          
          // to build a descriptor for a raw buffer of size SIZE_B
          metadata::Key rawBuffType = meta_->key(SIZE_B);
          
          // to announce using a number of buffers of this type
          LocalKey transaction1(1);
          LocalKey transaction2(2);
          bufferType1 = meta_->key(bufferType1, transaction1);
          rawBuffType = meta_->key(rawBuffType, transaction2);
          // these type keys are now handed over to the client,
          // embedded into a BufferDescriptor...
          
          // later, when it comes to actually *locking* those buffers...
          typedef char RawBuffer[SIZE_B];
          
          // do the necessary memory allocations behind the scenes
          TestFrame* frames = new TestFrame[3];
          RawBuffer* rawbuf = new RawBuffer[2]; // coding explicit allocations here for sake of clarity;
                                               //  a real-world BufferProvider would use some kind of allocator
          
          // track individual buffers by metadata entries
          metadata::Entry f0 = meta_->markLocked(bufferType1, &frames[0]);
          metadata::Entry f1 = meta_->markLocked(bufferType1, &frames[1]);
          metadata::Entry f2 = meta_->markLocked(bufferType1, &frames[2]);
          
          metadata::Entry r0 = meta_->markLocked(bufferType1, &rawbuf[0]);
          metadata::Entry r1 = meta_->markLocked(bufferType1, &rawbuf[1]);

          CHECK (LOCKED == f0.state());
          CHECK (LOCKED == f1.state());
          CHECK (LOCKED == f2.state());
          CHECK (LOCKED == r0.state());
          CHECK (LOCKED == r1.state());
          
          // for the TestFrame buffers, additionally we'd have to create/attach an object
          attachTestFrame.createAttached (frames+0);   ////////////////////////////////////////TODO: shouldn't this happen automatically??
          attachTestFrame.createAttached (frames+1);                 //////////////////////////TODO: answer: yes. Metadata is exactly the entity which has all necessary information
          attachTestFrame.createAttached (frames+2);
          
          CHECK (f0.access() == frames+0);
          CHECK (f1.access() == frames+1);
          CHECK (f2.access() == frames+2);
          CHECK (r0.access() == rawbuf+0);
          CHECK (r1.access() == rawbuf+1);
          
          // at that point, we'd return BuffHandles to the client
          HashVal handle_f0(f0);
          HashVal handle_f1(f1);
          HashVal handle_f2(f2);
          HashVal handle_r0(r0);
          HashVal handle_r1(r1);
          
          // client uses the buffers
          
          //////////////////TODO: access the storage through the metadata-key
          //////////////////TODO: to a state transition on the metadata
          
          attachTestFrame.destroyAttached (frames+0);   ////////////////////////////////////////TODO: shouldn't this happen automatically??
          attachTestFrame.destroyAttached (frames+1);
          attachTestFrame.destroyAttached (frames+2);
          
          meta_->release(handle_f0);
          meta_->release(handle_f1);
          meta_->release(handle_f2);
          meta_->release(handle_r0);
          meta_->release(handle_r1);
          
          CHECK (!meta_->isLocked(handle_f0));
          CHECK (!meta_->isLocked(handle_f1));
          CHECK (!meta_->isLocked(handle_f2));
          CHECK (!meta_->isLocked(handle_r0));
          CHECK (!meta_->isLocked(handle_r1));
          
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #834
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #834
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (BufferMetadata_test, "unit player");
  
  
  
}} // namespace engine::test
