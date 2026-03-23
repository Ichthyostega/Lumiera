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


#include "lib/error.hpp"
#include "lib/nocopy.hpp"
#include "vault/out/diagnostic-output-slot.hpp"
//#include "vault/out/output-slot-connection.hpp"

//#include <vector>
#include <utility>


namespace vault {
namespace out   {
  namespace err = lumiera::error;
  
//  using std::vector;
//  using Config = DiagnosticOutputSlot::Config;
  using std::move;
  using std::make_unique;
  
  namespace { // Implementation details of tracking
    
  }//(End)Impl details
  
  class DiagnosticOutputSlot::OutputTracker
    : public Config
    , util::NonCopyable
    {
    public:
      OutputTracker (Config&& config)
        : Config{move (config)}
        { }
    };
  
  
  class DiagnosticOutputSlot::DummyConnection
    : public OutputSlot::Connection
    , util::NonCopyable
    {
      OutputTracker& tracker_;
      
      
      /* === Connection API === */
      
      size_t
      getBufferSize()  const override
        {
          return tracker_.bufferSize;
        }
      
      Buff*
      claimBufferFor(FrameID frame)  override
        {
          UNIMPLEMENTED ("lock new buffer");
        }
      
      void
      publish (Buff* buff)  override
        {
          UNIMPLEMENTED ("publish buffer");
        }
      
      void
      release (Buff* buff)  override
        {
          UNIMPLEMENTED ("release buffer");
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

  
  
  /** */
  
  
  
  
}} // namespace vault::out
