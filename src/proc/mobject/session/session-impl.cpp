/*
  SessionImpl  -  holds the complete session data to be edited by the user
 
  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>
 
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 
* *****************************************************/


#include "proc/mobject/session/session-impl.hpp"
#include "proc/mobject/session/mobjectfactory.hpp"
#include "proc/mobject/placement.hpp"
#include "proc/mobject/mobject.hpp"
#include "lib/error.hpp"

namespace mobject {
namespace session {
  
  
  
  /** create a new empty session with default values.
   *  @note any exception arising while creating this
   *        default session will inevitably halt the
   *        system (and this is desirable)
   */
  SessionImpl::SessionImpl ()
    : SessionInterfaceModules()
    , Session( defaultsManager_
             , timelineRegistry_
             , sequenceRegistry_ )
    , contents_( MObject::create (defaultsManager_))
    , fixture_(new Fixture)
    {
      INFO (session, "new Session created.");
    }
  
  
  /** @internal used by SessionManager#clear
   *            discard all Session content,
   *            without touching global configuration.
   */
  void 
  SessionImpl::clear ()
  {
    try
      {
                          //////////////////////////////////////////////////////////////////////////TICKET #496
      }
    catch (...)
      {
        throw lumiera::error::Fatal ("unexpected exception while clearing the session"); ///////////TODO still required??
      }
  }
  
  
  bool 
  SessionImpl::isValid ()
  {
    UNIMPLEMENTED ("session self test");
    return false; // TODO
  }
  
  
  void 
  SessionImpl::attach (PMO& placement)
  {
    UNIMPLEMENTED ("add Placement to the current Session");
  }
  
  
  bool 
  SessionImpl::detach (PMO& placement)
  {
    UNIMPLEMENTED ("search and remove a given Placement from current Session");
    return false; // TODO
  }
  
  
  MObjectRef
  SessionImpl::getRoot()
  {
    UNIMPLEMENTED ("access and return the model root, packaged as MObject-ref");
  }

  
  
  PFix&
  SessionImpl::getFixture ()
  {
    return fixture_;
  }
  
  
  void
  SessionImpl::rebuildFixture ()
  { 
    UNIMPLEMENTED ("rebuild Fixture");
  }
  
  
  
}} // namespace mobject::session
