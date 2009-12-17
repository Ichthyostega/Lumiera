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
  
  /////////////////////////////////////////TODO temporary hack: use Meyer's singleton
  namespace {
    DefsManager&
    getDummyDefaultsManager()
    {
      static scoped_ptr<DefsManager> dummyInstance(0);
      if (!dummyInstance) dummyInstance.reset (new DefsManager);
      
      return *dummyInstance;
    }
  }
  /////////////////////////////////////////TODO temporary hack
  
  
  /** create a new empty session with default values.
   *  @note any exception arising while creating this
   *        default session will inevitably halt the
   *        system (and this is desirable)
   */
  SessionImpl::SessionImpl ()
    : Session( getDummyDefaultsManager() ),             ///////TODO temporary hack
      focusEDL_(0),
      edls(1),
      fixture(new Fixture),
      pIdx_( MObject::create (getDummyDefaultsManager()))  ////TODO temporary hack
    {
    }
  
  
  /** @internal used by SessionManager#clear
   *            discard all EDL content, without
   *            touching global configuration.
   */
  void 
  SessionImpl::clear ()
  {
    try
      {
        edls.clear();
        edls.resize(1);
        focusEDL_ = 0;
      }
    catch (...)
      {
        focusEDL_ = 0;
        throw lumiera::error::Fatal ("unexpected exception while clearing EDLs");
      }
  }
  
  
  bool 
  SessionImpl::isValid ()
  {
    UNIMPLEMENTED ("session self test");
    return false; // TODO
  }
  
  
  void 
  SessionImpl::add (PMO& placement)
  {
    UNIMPLEMENTED ("add Placement to the current EDL");
  }
  
  
  bool 
  SessionImpl::remove (PMO& placement)
  {
    UNIMPLEMENTED ("search and remove a given Placement from current EDL");
    return false; // TODO
  }
  
  /// @deprecated should not grant direct access to EDL objects
  EDL&
  SessionImpl::currEDL ()
  {
    ASSERT (focusEDL_ < edls.size());
    return  edls[focusEDL_];
  }
  
  
  PFix&
  SessionImpl::getFixture ()
  {
    return fixture;
  }
  
  
  void
  SessionImpl::rebuildFixture ()
  { 
    UNIMPLEMENTED ("rebuild Fixture");
  }
  
  
  
}} // namespace mobject::session
