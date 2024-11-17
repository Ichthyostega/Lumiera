/*
  ENGINE-OBSERVER.hpp  -  collection and maintenance of engine performance indicators

   Copyright (C)
     2023,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file engine-observer.hpp
 ** Render Engine performance data collection service. Data indicative of the
 ** current operational state is emitted at various levels of processing as
 ** synchronous notification calls. The information transmitted must be offloaded
 ** quickly for asynchronous processing to generate the actual observable values.
 ** 
 ** @see scheduler.hpp
 ** @see job-planning.hpp
 ** @see Activity::Verb::WORKSTART
 ** 
 ** @todo WIP-WIP-WIP 10/2023 »Playback Vertical Slice« created as a stub
 ** @todo design and implement the EngineObserver as publisher-subscriber... ////////////////////////////////TICKET #1347 : design EngineObserver
 ** 
 */


#ifndef SRC_VAULT_GEAR_ENGINE_OBSERVER_H_
#define SRC_VAULT_GEAR_ENGINE_OBSERVER_H_


#include "lib/error.hpp"
#include "vault/gear/block-flow.hpp"
#include "vault/gear/scheduler-commutator.hpp"
#include "vault/gear/scheduler-invocation.hpp"
#include "lib/symbol.hpp"
#include  "lib/nocopy.hpp"
//#include "lib/util.hpp"

//#include <string>
#include <utility>
#include <array>


namespace vault{
namespace gear {

  using lib::Symbol;
//  using util::isnil;
//  using std::string;
  using std::move;
  
  /**
   * Low-level Render Engine event — abstracted storage base.
   */
  class EngineEvent
    {
    protected:
      static size_t constexpr RAW_SIZ = 3;
      using Storage = std::array<int64_t, RAW_SIZ>;
      
      template<class DAT>
      union Payload
        {
          static_assert (sizeof(DAT) <= RAW_SIZ * sizeof(int64_t));
          
          Storage raw;
          DAT    data;
          
          Payload()              : raw{0} { }
          Payload (DAT const& d) : data{d} { }
          Payload (DAT    && dr) : data{move(dr)} { }
          
          // default copy and assignment acceptable
          
          DAT const& operator= (DAT const& d) { data = d;        return data; }
          DAT const& operator= (DAT    && dr) { data = move(dr); return data; }
          
          operator Storage&()  { return raw; }
          operator Storage&&() { return move(raw); }
        };
      
      /** base init for derived classes to implant custom payload */
      EngineEvent (Symbol msgID, Storage&& payload)
        : message{msgID}
        , storage_{move (payload)}
        { }
      
    public:
      EngineEvent()
        : message{Symbol::BOTTOM}
        , storage_{0}
        { }
      
      // default copy and assignment acceptable
      
      Symbol message;
      
    private:
      Storage storage_;
    };
  
  
  
  /**
   * Collector and aggregator for performance data.
   * @todo WIP-WIP 10/2023 - stub as placeholder for later development   ////////////////////////////////////TICKET #1347 : design EngineObserver
   * @see Scheduler
   */
  class EngineObserver
    : util::NonCopyable
    {
      
    public:
      explicit
      EngineObserver()
        { }
      
      void
      dispatchEvent (size_t /*address*/, EngineEvent /*event*/)
        {
          /* TICKET #1347 actually move this event into a dispatcher queue */
        }
    };
  
  
  
}}// namespace vault::gear
#endif /*SRC_VAULT_GEAR_ENGINE_OBSERVER_H_*/
