/*
  EVENT-LOG.hpp  -  test facility to verify the occurrence of expected events

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

*/


/** @file event-log.hpp
 ** Support for verifying the occurrence of events from unit tests.
 ** Typically used within special rigging and instrumentation for tests,
 ** the [EventLog] allows to record invocations and similar events. It is
 ** implemented as a "PImpl" to allow sharing of logs. The front-end used
 ** for access offers a query facility, so the test code may express some
 ** expected patterns of incidence and verify match or non-match.
 ** 
 ** Failure of match deliberately throws an assertion failure, in order to
 ** deliver a precise indication what part of the condition failed.
 ** 
 ** @todo as of 11/2015 this is complete WIP-WIP-WIP
 ** 
 ** @see ////TODO_test usage example
 ** 
 */


#ifndef LIB_TEST_EVENT_LOG_H
#define LIB_TEST_EVENT_LOG_H


#include "lib/error.hpp"
//#include "lib/time/timevalue.hpp"

//#include <boost/lexical_cast.hpp>
//#include <string>



namespace lib {
namespace test{
  
//  using lib::Literal;
//  using std::string;
//  using std::rand;
  
  
  /**
   * Helper to log and verify the occurrence of events.
   */
  class EventLog
    {
      
    };
  
  
  
}} // namespace lib::test
#endif /*LIB_TEST_EVENT_LOG_H*/
