/*
  dummy-session-connection.hpp  -  scaffolding placeholder to drive the GUI-Session connection

  Copyright (C)         Lumiera.org
    2016,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file dummy-session-connection.hpp
 ** Dummy and scaffolding to help development of the UI - Session connection.
 ** This singleton service can be accessed as placeholder to stand-in for all those
 ** parts of the application, which are to defined in the very attempt this placeholder
 ** is used for...
 ** - it can fake some session content
 ** - it may expose some fake command implementations
 ** - and all this together with suitable diagnostic facilities
 ** 
 ** The plan is to push this helper along, as we proceed from the UI down towards the session.
 ** In the end, this placeholder service may be inserted into the session implementation proper,
 ** until we have the ability to load a persistent session from storage.
 ** 
 ** @todo WIP as of 12/2016         //////////////////////////////////////////////////////////////////////TICKET #1042
 ** 
 ** @see ui-bus.hpp
 ** @see dummy-session-connection.cpp implementation
 ** 
 */

#ifndef PROC_MOBJECT_SESSION_DUMMY_SESSION_CONNECTION_H
#define PROC_MOBJECT_SESSION_DUMMY_SESSION_CONNECTION_H

#include "lib/error.hpp"
#include "lib/depend.hpp"
#include "lib/nocopy.hpp"
#include "lib/util.hpp"

#include <string>

namespace proc {
namespace mobject {
namespace session {
  
//using util::isnil;
  using std::string;
  
  /**
   * Basic (abstracted) view of...
   * 
   * @see SomeSystem
   * @see DummySessionConnection_test
   */
  class DummySessionConnection
    : util::NonCopyable
    {
      string nothing_;

      DummySessionConnection();
     ~DummySessionConnection();

      friend class lib::DependencyFactory;

    public:
      /** access point to set up the scaffolding.
       * @internal this is a development helper and will be remoulded all the time.
       */
      static lib::Depend<DummySessionConnection> instance;
      
      
      /* == X interface for == */

      /* == Adapter interface for == */

#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #1042
      void setSolution (string const& solution = "")
        {
          UNIMPLEMENTED ("tbw");
          if (isDeaf())
          this->transmogrify (solution);
        }
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #1042
    };
  
  
  
}}} // namespace proc::mobject::session
#endif /*SRC_PROC_MOBJECT_SESSION_DUMMY_SESSION_CONNECTION_HPP_*/
