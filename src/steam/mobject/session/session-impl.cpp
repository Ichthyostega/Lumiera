/*
  SessionImpl  -  holds the complete session data to be edited by the user

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file session-impl.cpp
 ** Implementation of the global session datastructure
 ** @todo WIP implementation of session core from 2010
 ** @todo as of 2016, this effort is considered stalled but basically valid
 */


#include "steam/mobject/session/session-impl.hpp"
#include "steam/mobject/session/mobjectfactory.hpp"
#include "steam/mobject/session/query-focus.hpp"
#include "steam/mobject/placement.hpp"
#include "steam/mobject/mobject.hpp"
#include "lib/error.hpp"


namespace steam {
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
             , elementQueryAPI_
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
        throw error::Fatal ("unexpected exception while clearing the session");          ///////////TODO still required??
      }
  }
  
  
  bool 
  SessionImpl::isValid ()
  {
    return true;          //////////////////////////////////////////////////////////////////////////TICKET #694
  }
  
  
  
  /** attach a copy within the scope
   *  of the current QueryFocus point
   *  @return reference to the newly created
   *          instance (placement) which was attached
   *          below the position given by current focus
   */
  MObjectRef 
  SessionImpl::attach (PMO const& placement)
  {
    MObjectRef newAttachedInstance;
    RefPlacement attachmentPoint = QueryFocus().currentPoint();
    newAttachedInstance.activate(
        contents_.insert (placement, attachmentPoint));
    return newAttachedInstance;
  }
  
  
  /** detach the denoted object (placement) from model,
   *  together with any child objects contained in the
   *  scope of this placement. 
   *  @note as a sideeffect, the current QueryFocus
   *        is moved to the scope containing the
   *        object to be removed
   *  @throw error::Invalid when attempting to kill root
   *  @return \c true if actually removing something
   */
  bool 
  SessionImpl::detach (PMO const& placement)
  {
    bool is_known = contents_.contains (placement);
    if (is_known)
      {
        if (Scope(placement).isRoot())
          throw error::Invalid ("Can't detach the model root."
                               , LERR_(INVALID_SCOPE));
        
        QueryFocus currentFocus;
        currentFocus.shift (Scope(placement).getParent());
        contents_.clear (placement);
      }
    ENSURE (!contents_.contains (placement));
    return is_known;
  }
  
  
  MObjectRef
  SessionImpl::getRoot()
  {
    MObjectRef refRoot;
    refRoot.activate (contents_.getRoot());
    return refRoot;
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
  
  
  
}}} // namespace steam::mobject::session
