/*
  DiagnosticOutputSlot  -  helper for testing against the OutputSlot interface

   Copyright (C)
     2026,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file diagnostic-output-slot.cpp
 ** Internal implementation parts of a state-tracking dummy implementation
 ** of the OutputSlot interface and framework.
 */


#include "lib/integral.hpp"
#include "lib/nocopy.hpp"
#include "vault/out/diagnostic-output-slot.hpp"
//#include "vault/out/output-slot-connection.hpp"
#include "vault/mem/buffhandle.hpp"
#include "vault/mem/naive-buffer-setup.hpp"
#include "vault/real-clock.hpp"
#include "lib/util.hpp"

//#include <vector>
#include <utility>
#include <algorithm>
#include <functional>
#include <map>

using util::contains;
//  using std::vector;
//  using Config = DiagnosticOutputSlot::Config;
using std::byte;
using std::move;
using std::make_unique;
using std::make_shared;
using std::function;

using vault::mem::NaiveBufferSetup;
using vault::mem::BuffHandle;
using vault::mem::BuffDescr;

namespace vault {
namespace out   {
  namespace err = lumiera::error;
  
  namespace { // Implementation details of tracking
    inline void
    zeroFill (void* buf, size_t siz)
    {
      auto begin = static_cast<byte*> (buf);
      std::fill (begin, begin+siz, byte(0));
    }
  }//(End)Impl details
  
  class DiagnosticOutputSlot::OutputTracker
    : public Config
    , util::NonCopyable
    {
      std::vector<diagn::FeedLog> feed_;
      NaiveBufferSetup buffProvider_;
      BuffDescr bufferType_;
      

    public:
      OutputTracker (Config&& config)
        : Config{move (config)}
        , feed_{config.numDataFeeds}
        , buffProvider_{}
        , bufferType_{buffProvider_.getDescriptorFor (bufferSize
                                                     ,mem::TypeHandler{[siz=bufferSize](void* buff){ zeroFill (buff, siz);}
                                                                      ,[](void*){ /* do nothing on release */   }
                                                                      })}
        { }
        
        /** configurable functor: what is used as _current time_
         * @remark allows the test framework to mark "now" as some
         *         specific fixed time; default is to return system time
         */
        function<Time()> currentTime{[]{ return RealClock::now(); }};
        
        
        BuffHandle
        getNewBuffer()
          {
            return bufferType_.lockBuffer();
          }
        
        diagn::FeedLog const&
        getFeed (uint feedNr)
          {
            REQUIRE (feedNr < feed_.size());
            return feed_[feedNr];
          }
    };
  
  /**
   * Front-end for access by test code.
   * @note tolerates access to arbitrary feeds
   */
  diagn::FeedLog const&
  OutputDiagnostic::feed (uint feedNr)
  {
    if (feedNr < dos_.tracker_->numDataFeeds)
      return dos_.tracker_->getFeed (feedNr);
    else
      return lib::NullValue<diagn::FeedLog>::get();
  }

  
  
  class DiagnosticOutputSlot::DummyConnection
    : public OutputSlot::Connection
    {
      OutputTracker& tracker_;
      
      std::map<Buff*, BuffHandle> buffIdx_;
      
      
      /* === Connection API === */
      
      size_t
      getBufferSize()  const override
        {
          return tracker_.bufferSize;
        }
      
      Buff*
      claimBufferFor(FrameID frame)  override
        {
          BuffHandle handle = tracker_.getNewBuffer();
          REQUIRE (handle.isValid());
          Buff* buff = handle.rawStorage();
          ENSURE (not contains (buffIdx_, buff));
          buffIdx_.insert ({buff, handle});
          UNIMPLEMENTED ("track new buffer");
          return buff;
        }
      
      void
      publish (Buff* buff)  override
        {
          REQUIRE (contains (buffIdx_,buff));
          auto handle = buffIdx_.find(buff)->second;
          handle.emit();
          UNIMPLEMENTED ("track publish buffer");
        }
      
      void
      release (Buff* buff)  override
        {
          REQUIRE (contains (buffIdx_,buff));
          auto handle = buffIdx_.find(buff)->second;
          handle.release();
          UNIMPLEMENTED ("track release buffer");
        }
      
      void
      shutDown()  override
        {
          TODO ("Anything to record at connection shut-down?");
        }
      
      
      
    public:
      DummyConnection (OutputTracker& tracker)
        : tracker_{tracker}
        { }
    };
  
  
  
  DiagnosticOutputSlot::~DiagnosticOutputSlot() { }  // emit destructors for hidden implementation here....
  
  DiagnosticOutputSlot::DiagnosticOutputSlot (Config config)
    : DiagnosticOutputSlot{setupOutputTracker(move (config))}
    { }
  
  /**
   * @internal setup and construction sequence for the diagnostic tracking.
   * This tricky call sequence is required to resolve two conflicting goals
   * - we want a complete one-shot setup and wiring in the constructor.
   * - we want DiagnosticOutputSlot be usable like an OutputSlot,
   *   even when this implies to create a slicing copy (its a value object!)
   * These requirements imply that OutputSlot should be the (first) base object,
   * but should also be initialised with the OutputTracker, which however can
   * only be initialised _after_ the base object.
   * 
   * Yet the OutputTracker is installed as a »PImpl« and managed by a shared-ptr anyway,
   * so that this contradiction can be circumvented by constructing it first, as a constructor
   * argument \a trackingSetup prepared through a chained constructor call. It is thus visible
   * in the complete scope of the constructor, so that we can pass it to the base class
   * constructor, and later then move it into the final location in the derived class. 
   */
  DiagnosticOutputSlot::DiagnosticOutputSlot (shared_ptr<OutputTracker> trackingSetup)
    : OutputSlot{setupTrackingConnections (*trackingSetup)}
    , tracker_{move(trackingSetup)}
    { }
      
  shared_ptr<DiagnosticOutputSlot::OutputTracker>
  DiagnosticOutputSlot::setupOutputTracker (Config&& config)
  {
    return make_shared<OutputTracker> (move (config));
  }
  
  unique_ptr<OutputSlot::Allocation>
  DiagnosticOutputSlot::setupTrackingConnections (DiagnosticOutputSlot::OutputTracker& outputTracker)
  {
    using AllocState = OutputSlot::AllocState<DummyConnection>;
    using ConStorage = AllocState::ConnectionStorage;
    
    return make_unique<AllocState> (outputTracker.numDataFeeds
                                   ,[&](ConStorage& storage){ storage.create<DummyConnection> (outputTracker); }
                                   );
  }
  
  
  
}} // namespace vault::out
