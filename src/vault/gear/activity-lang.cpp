/*
  ActivityLang  -  definition language framework for scheduler activities

   Copyright (C)
     2023,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file activity-lang.cpp
 ** Supporting implementation for the scheduler activity language framework.
 ** @note most of the language processing is defined as inline functions
 **       and uses fixed-size data storage in a dedicated custom allocator.
 **       Timing measurements confirmed the benefits, reducing invocations
 **       from ~50µs to <5µs in optimised mode, and this indeed matters,
 **       as the scheduler can be considered performance sensitive code.
 */


#include "vault/gear/activity-lang.hpp"
#include "lib/format-obj.hpp"
#include "lib/symbol.hpp"
#include "vault/gear/scheduler.hpp"//////////////////////////////////////////////////////////////TODO extract -> scheduler.cpp

#include <string>
#include <boost/functional/hash.hpp> ////////////////////////////////////////////////////////////TODO should be in a scheduler translation-unit / extract scheduler-API

using std::string;
using lib::Symbol;
using lib::time::Time;
using lib::time::TimeValue;


namespace vault{
namespace gear {
  namespace activity {
    Hook::~Hook() { } // emit VTable here...
  }
  ///////////////////////////////////////////////////////////////////////////////////////////////TODO should be in a scheduler translation-unit / extract scheduler-API
  HashVal
  hash_value (ManifestationID const& id)
    {
      return boost::hash_value (uint32_t{id});
    }
  
  Symbol WorkTiming::WORKSTART{"WorkStart"};
  Symbol WorkTiming::WORKSTOP {"WorkStop"};
  
  ///////////////////////////////////////////////////////////////////////////////////////////////TODO extract scheduler-API
  
  
  Activity::operator string()  const
  {
    return string{"Act("}
         + showVerb()
         + util::showAdr(this)
         + ": "
         + showData()
         + "; ⤵ "
         + (next? next->showVerb()+util::showAdr(*next)
                : util::BOTTOM_INDICATOR)
         + ")";
  }

  string
  Activity::showVerb()  const
  {
    switch (verb_) {
      case INVOKE:    return "INVOKE";
      case WORKSTART: return "WORKSTART";
      case WORKSTOP:  return "WORKSTOP";
      case NOTIFY:    return "NOTIFY";
      case GATE:      return "GATE";
      case POST:      return "POST";
      case FEED:      return "FEED";
      case HOOK:      return "HOOK";
      case TICK:      return "TICK";
      default: NOTREACHED ("uncovered Activity verb diagnostics.");
      }
  }

  string
  Activity::showData()  const
  {
    switch (verb_) {
      case INVOKE:
        return util::showPtr (data_.invocation.task)
             + ", "
             + util::toString(TimeValue{data_.invocation.time});
      case WORKSTART:
      case WORKSTOP:
        return util::toString(TimeValue{data_.timing.instant})
             + ", quality="
             + util::toString(data_.timing.quality);
      case NOTIFY:
        return util::showAdr(data_.notification.target)
             + ", timing:"
             + util::toString(Time{data_.notification.timing});
      case GATE:
        return "<"
             + util::toString(data_.condition.rest)
             + ", until "
             + util::toString(Time{data_.condition.dead});
      case POST:
        return "["
             + util::toString(Time{data_.timeWindow.life})
             + "…"
             + util::toString(Time{data_.timeWindow.dead})
             + "]";
      case FEED:
        return "{"
             + util::toString(data_.feed.one)
             + "::"
             + util::toString(data_.feed.two)
             + "}";
      case HOOK:
        return util::showPtr (data_.callback.hook)
             + "("
             + util::toString(data_.callback.arg)
             + ")";
      case TICK:
        return "◆ ";
      default:
        NOTREACHED ("uncovered Activity verb in activation function.");
      }
  }
  
  
}} // namespace vault::gear
