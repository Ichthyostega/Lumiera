/*
  BufferProviderProtocol(Test)  -  demonstration of buffer provider usage cycle

   Copyright (C)
     2011,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file buffer-provider-protocol-test.cpp
 ** unit test \ref BufferProviderProtocol_test
 */


#include "lib/error.hpp"
#include "test/run.hpp"
#include "test/test-helper.hpp"
#include "test/tracking-dummy.hpp"
#include "steam/engine/diagnostic-buffer-provider.hpp"
#include "steam/engine/test-rand-ontology.hpp"
#include "steam/engine/testframe.hpp"
#include "lib/iter-zip.hpp"

#include <array>

using util::isSameObject;
using lib::HashVal;
using lib::zip;


namespace steam {
namespace engine{
namespace test  {
  
  using LERR_(LIFECYCLE);
  
  
  namespace { // Test fixture
    
    const uint TEST_ELMS = 50;
    
    
    HashVal
    do_some_computation (TestFrame const& src, TestFrame& work, int64_t* data)
    {
      REQUIRE (data);
      ont::Param param = src.getChecksum();
      ont::manipulateFrame (&work, &src, param);
      for (uint i=0; i<TestFrame::BUFFSIZ; ++i)
        *(data+i) = work.data()[i];
      return work.getChecksum();
    }
  }
  
  
  
  /**************************************************************************//**
   * @test document the usage patterns of the »Buffer Provider Protocol«
   * Notably this protocol is used by the Render Engine during recursive `pull()`
   * calls to claim and release the working buffers for each step; furthermore,
   * also the output buffers are packaged as `BuffHandle` and used through this
   * protocol.
   * 
   * However, the actual `BufferProvider` implementation used here is instrumented
   * for diagnostics and does not actually _manage_ buffers; rather each buffer
   * is allocated on the heap and never released, so that any allocation and
   * all buffer contents can be verified after the fact.
   */
  class BufferProviderProtocol_test : public Test
    {
      virtual void
      run (Arg)
        {
          seedRand();
          
          verifySimpleUsage();
          verifyRenderingUse();
          verifyObjectEmbedding();
        }
      
      
      void
      verifySimpleUsage()
        {
          // Create Test fixture.
          // In real usage, a suitable memory/frame/buffer provider
          // will be preconfigured, depending on the usage context
          DiagnosticBufferProvider provider;
          
          BuffHandle buff = provider.lockBufferFor<TestFrame>();
          CHECK (buff.isValid());
          CHECK (sizeof(TestFrame) <= buff.size());
          buff.accessAs<TestFrame>() = testData(0);
          
          TestFrame& content = buff.accessAs<TestFrame>();
          CHECK (testData(0) == content);
          
          buff.emit();
          buff.release();
          CHECK (not buff.isValid());
          VERIFY_ERROR (LIFECYCLE, buff.accessAs<TestFrame>() );
          
          auto inspect = watch(provider);
          CHECK (inspect.was_used (buff));
          CHECK (inspect.was_released(buff));
          
          CHECK (testData(0) == inspect.accessMemory(buff));
        }
      
      
      /** @test demonstrate the full sequence of invocations encountered during rendering:
       *      - as a first step, _buffer types_ are declared (usually by the Builder)
       *      - before start of computations, required amount of buffers is _announced_
       *      - actual buffers are then claimed from within the ongoing computation
       *      - buffer handles can be passed on and re-assigned
       *      - buffers with computed results are _marked emitted_
       *      - each step not only creates new buffers, but also _releases_ old ones.
       * @remark while the control structure in this test is represented by a simple `for`-loop,
       *   instead of the complicated recursive call-down found in the actual Render Engine,
       *   the actual computations use a test-scheme with chained hash values, which allows
       *   to prove that all computations happened exactly in the planned order. This
       *   technique was developed for [verifying render nodes](\ref node-devel-test.cpp)
       *   and relies on TestFrame objects representing a buffer filled with random yet
       *   deterministic data. Each step in this [dummy computation](\ref do-some_computation)
       *   - uses a source data buffer
       *   - reads the _checksum_ of this source buffer
       *   - uses this checksum as a _parameter_ for a »filtering operation«
       *   - this filter or manipulation operation chains each source value with the parameter
       *   - the resulting result values are then extracted from TestFrame and copied into
       *     another data buffer (thereby widening from 8bit data to 64bit data)
       *   - furthermore, each step computes the checksum of the result buffer
       *   - and stores this checksum into a mark buffer
       *   - note that the result buffer is passed on as source buffer to the next step.
       *   Once this computation scheme is carried out, the checksum from the last result buffer
       *   is retrieved. All buffers are already marked as discarded at that point, yet due to
       *   the DiagnosticBufferProvider used for the test, the actual memory blocks were not
       *   really deleted yet, and their contents can be inspected and verified after the fact.
       *   This verification reinvokes the computation steps to validate the captured checksums,
       *   which ensures that, overall
       *   - buffers are not mixed up
       *   - the buffer identities are maintained in the correct way
       *   - data in buffers is not corrupted or re-used in uncontrolled ways
       *   - all protocol steps (locking, emitting, releasing) happen in the expected order
       */
      void
      verifyRenderingUse()
        {
          // Test fixture: allows to track/verify allocations after the fact
          DiagnosticBufferProvider provider;
          
          using DataBuff = std::array<int64_t, TestFrame::BUFFSIZ>;
          constexpr auto BUFFSIZ = sizeof(DataBuff);
          
          BuffDescr desc1 = provider.getDescriptor<size_t>();
          BuffDescr desc2 = provider.getDescriptor<TestFrame>();   // note: implies also sizeof(TestFrame)
          BuffDescr desc3 = provider.getDescriptorFor(BUFFSIZ);    //       Can also request fixed sized raw storage.
          CHECK (desc1.isValid());
          CHECK (desc2.isValid());
          CHECK (desc3.isValid());
          
          uint num1 = desc1.announce(TEST_ELMS);
          uint num2 = desc2.announce(TEST_ELMS + 1);
          uint num3 = desc3.announce(TEST_ELMS * 2); // announce more than we actually use (which is tolerated)
          CHECK (num1 == TEST_ELMS);
          CHECK (num2 == TEST_ELMS + 1);
          CHECK (num3 == TEST_ELMS * 2);
          
          // emulate a progressive processing
          BuffHandle workBuff = desc2.lockBuffer();
          workBuff.accessAs<TestFrame>() = testData(0);
          for (uint i=0; i<TEST_ELMS; ++i)
            {
              BuffHandle srcBuff = workBuff;
              workBuff = desc2.lockBuffer();
              
              BuffHandle markBuff = desc1.lockBuffer();
              BuffHandle dataBuff = desc3.lockBuffer();
              CHECK (srcBuff.isValid());
              CHECK (markBuff.isValid());
              CHECK (workBuff.isValid());
              CHECK (dataBuff.isValid());
              
              TestFrame& src  = srcBuff.accessAs<TestFrame>();
              size_t&    mark = markBuff.accessAs<size_t>();
              TestFrame& work = workBuff.accessAs<TestFrame>();
              DataBuff&  data = dataBuff.accessAs<DataBuff>();
              
              mark = do_some_computation (src,work, &data[0]);
              
              srcBuff.emit();
              dataBuff.emit();
              markBuff.emit();
              
              srcBuff.release();
              dataBuff.release();
              markBuff.release();
              // note : workBuff passed to next iteration
            }
          CHECK (workBuff.isValid());
          auto endSum = workBuff.accessAs<TestFrame>().getChecksum();
          workBuff.release(); // note: not every buffer need be emitted.
          CHECK (!workBuff);
          
          // Computation complete now — verify buffer management....
          auto inspect = watch(provider);
          CHECK (inspect.created.cnt() == 3*TEST_ELMS + 1);
          CHECK (inspect.was_used (workBuff));
          CHECK (inspect.was_released (workBuff));
          CHECK (not inspect.was_emitted (workBuff));
          CHECK (inspect.all_buffers_released());
          
          // peek into buffer memory...
          // to prove that that the complete computation chain was
          // in fact performed, and was using the memory as intended
          uint last = inspect.released.cnt() - 1;
          diagn::Block lastWork = inspect.released[last];
          diagn::Block lastMark = inspect.released[last-1];
          diagn::Block lastData = inspect.released[last-2];
          
          // due to the way this chained-hash dummy computation was defined,
          // the last data buffer should contain the same random numbers as the last work buffer...
          auto& workBuffContent = lastWork.accessAs<TestFrame>().data();
          auto& dataBuffContent = lastData.accessAs<DataBuff>();
          
          for (auto [c,v] : zip(workBuffContent,dataBuffContent))
            CHECK (c == v);
          
          // furthermore, the last recorded checksum mark
          // should match the checksum of the last work buffer,
          // because that is where the last computation was carried out.
          CHECK (endSum == lastMark.accessAs<HashVal>());
          
          // we can also verify that all data in emitted buffers is correct,
          // by re-computing the checksum for each step, using data in the buffers
          for (uint i=0; i<TEST_ELMS; ++i)
            {
              diagn::Block srcBuff  = inspect.emitted[i*3 + 0];
              diagn::Block markBuff = inspect.emitted[i*3 + 2];
              TestFrame const& src = srcBuff.accessAs<TestFrame>();  // note const -- can not corrupt data in memory
              ont::Param param = src.getChecksum();
              TestFrame newWorkBuff;
              ont::manipulateFrame (&newWorkBuff, &src, param);
              
              // the checksum recoded during the test run matches the re-computed sum,
              // based on combining data in memory for a second time with the same chained-hash function
              CHECK (markBuff.accessAs<HashVal>() == newWorkBuff.getChecksum());
            }
        }
      
      
      
      /** @test automatic embedding of structures into a buffer
       *      - contents of a raw buffer are never touched by the BufferProvider
       *      - yet it is possible to define a _buffer type_ to _emplace_
       *        some arbitrary type into the buffer; such an embedded instance
       *        is then managed alongside with the buffer state transitions
       *      - we can even define custom handler functions to prepare
       *        arbitrary structures within the buffer — or engage into
       *        whatever kind of havoc we like (you have been warned)
       *      - when invoking an _emergency clean-up_, destructor functors
       *        can optionally invoked, or skipped (the latter is the default)
       * @remark this test uses the [tracking test-dummy](\ref lib::test::Tracker),
       *   which records each ctor and dtor call into an lib::test::EventLog.
       *   This allows us to play through several scenarios and verify
       *   that objects are created and (not) destroyed, all as expected.
       * @see test-tracking-test.cpp
       * @see event-log-test.cpp
       */
      void
      verifyObjectEmbedding()
        {
          DiagnosticBufferProvider provider;
          
          using ::test::Tracker;
          auto& log = Tracker::log;
          log.clear();
          
          BuffDescr rawBuff = provider.getDescriptorFor(sizeof(Tracker));
          BuffDescr objBuff = provider.getDescriptor<Tracker>();
          CHECK (rawBuff.buffSize() == objBuff.buffSize());
          
          
          { // Case-1 : Raw buffer does not create any embedded instance
            log.event("Case-1");
            BuffHandle handle = provider.lockBuffer(rawBuff);
            CHECK (handle.size() >= sizeof(Tracker));
            CHECK (handle.isAllotted());
            // But no object has been created yet....
            CHECK (log.ensureNot("ctor")
                      .afterEvent("Case-1"));
            
            // explicitly placement-construct object into the buffer
            new(handle.rawStorage()) Tracker{55};
            Tracker& emplaced = handle.accessAs<Tracker>();
            CHECK (emplaced.val == 55);
            CHECK (log.verify("Case-1")
                      .beforeCall("ctor").on(&emplaced).arg(55));
            
            handle.release();
            CHECK (not handle.isAllotted());
            
            // Buffer is disposed now,
            // but emplaced object was not destroyed automatically
            CHECK (log.ensureNot("dtor")
                      .afterCall("ctor").arg(55));
          }
          
          
          { // Case-2 : Buffer with object instance automatically emplaced
            log.event("Case-2");
            CHECK (log.ensureNot("ctor").after("Case-2"));
            
            // allot buffer and emplace Tracker instance automatically
            BuffHandle handle = provider.lockBuffer(objBuff);
            Tracker& emplaced = handle.accessAs<Tracker>();
            CHECK (log.verify("Case-2")
                      .beforeCall("ctor").on(&emplaced).arg());
            
            int randomVal{emplaced.val};
            CHECK (0 < randomVal and randomVal < 1000);
            // NOTE: buffer access is actually an unchecked force-cast!
            int something = handle.accessAs<int>();
            CHECK (something == randomVal);
            CHECK (std::is_standard_layout_v<Tracker>);
            
            CHECK (log.ensureNot("dtor").afterEvent("Case-2"));
            handle.emit();
            CHECK (not handle);          // no longer accessible
            CHECK (handle.isAllotted()); // while buffer contents are still alive
            CHECK (log.ensureNot("dtor").afterEvent("Case-2"));
            
            // Emplaced object is destroyed when releasing buffer
            handle.release();
            CHECK (not handle.isAllotted());
            CHECK (log.verify("Case-2")
                      .beforeCall("ctor").arg()
                      .beforeCall("dtor").arg(randomVal));
          }
          
          
          { // Case-3 : Emergency clean-up
            log.event("Case-3");
            
            BuffHandle buf1 = provider.lockBuffer(objBuff);
            BuffHandle buf2 = provider.lockBuffer(objBuff);
            
            auto& o1 = buf1.accessAs<Tracker>();
            auto& o2 = buf2.accessAs<Tracker>();
            CHECK (log.verify("Case-3")
                      .beforeCall("ctor").on(&o1).arg()
                      .beforeCall("ctor").on(&o2).arg());
            
            int i1 = o1.val;
            CHECK (i1 > 0);
            o2.val = -1; // so we can tell them apart
            
            // Emergency clean-up can optionally invoke the dtor
            CHECK (buf1.isAllotted());
            CHECK (buf2.isAllotted());
            provider.emergencyCleanup(buf1, true); // invoke dtor
            buf2.emergencyCleanup();               // don't invoke dtor
            CHECK (not buf1.isAllotted());
            CHECK (not buf2.isAllotted());
            
            // only dtor for o1 was invoked
            CHECK (log.verify("Case-3")
                      .beforeCall("ctor")
                      .beforeCall("dtor").arg(i1));
            // while buf2 was discarded without invoking dtor of o2
            CHECK (log.ensureNot("Case-3")
                      .beforeCall("ctor")
                      .beforeCall("dtor").arg(-1));
          }
          
          
          { // Case-4 : explicitly given ctor/dtor-λ
            log.event("Case-4");
            using Arr = std::array<Tracker,2>;
            TypeHandler treat{[](void* buff){ // invoked on each buffer created from that type
                                              Tracker::log.event("create two...");
                                              new(buff) Arr{};
                                            }
                             ,[](void* buff){ // invoked whenever releasing a buffer of that type
                                              static_cast<Arr*>(buff)->~Arr();
                                              Tracker::log.event("both destroyed");
                                            }
                             };
            
            BuffDescr special = provider.getDescriptorFor(sizeof(Arr), treat);
            CHECK (special.isValid());
            CHECK (special.buffSize() == sizeof(Arr));
            CHECK (HashVal(special) != HashVal(rawBuff));
            CHECK (HashVal(special) != HashVal(objBuff));
            
            // nothing happened yet...
            CHECK (log.ensureNot("create two..."));
            
            // allocate a buffer as »instance« of this buffer type...
            BuffHandle buff = special.lockBuffer();
            CHECK (buff.isValid());
            CHECK (log.verify("Case-4")
                      .beforeEvent("create two...")
                      .beforeCall("ctor").arg()
                      .beforeCall("ctor").arg());
            // so buffer was called and two objects created, but not destroyed yet
            CHECK (log.ensureNot("Case-4").before("ctor").before("dtor"));
            
            // can use buffer with embedded structure now...
            Arr& inlay = buff.accessAs<Arr>();
            CHECK (0 < inlay[0].val);
            CHECK (0 < inlay[1].val);
            int i1 = inlay[0].val;
            inlay[1].val = -55; // so we can identify the dtor call in the log
            
            // complete buffer usage cycle...
            buff.emit();
            CHECK (not buff);
            CHECK (buff.isAllotted());
            // objects are still alive....
            CHECK (log.ensureNot("Case-4").before("both destroyed"));
            //
            buff.release();
            CHECK (not buff.isAllotted());
            CHECK (log.verify("Case-4")
                      .beforeEvent("create two...")
                      .before("ctor")
                      .before("ctor")
                      .before("dtor")
                      .before("dtor")
                      .beforeEvent("both destroyed"));
            // Order of individual ctor/dtor calls is implementation defined,
            // yet the overall sequence is guaranteed...
            CHECK (log.verify("Case-4")
                      .before("ctor")
                      .before("dtor").arg(i1)
                      .beforeEvent("both destroyed"));
            CHECK (log.verify("Case-4")
                      .before("ctor")
                      .before("dtor").arg(-55)
                      .beforeEvent("both destroyed"));
          }
          
          cout << "____Tracker-Log_______________\n"
               << util::join(Tracker::log,      "\n")
               << "\n───╼━━━━━━━━━━━╾──────────────"<<endl;
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (BufferProviderProtocol_test, "unit engine");
  
  
  
}}} // namespace steam::engine::test
