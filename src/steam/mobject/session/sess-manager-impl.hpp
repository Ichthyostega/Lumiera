/*
  SESS-MANAGER-IMPL.hpp  -  global session access and lifecycle

   Copyright (C)
     2009,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file sess-manager-impl.hpp
 ** Implementation facility for session management.
 ** Users are assumed to access the session itself through a smart-ptr, which happens
 ** to be the SessManager. Thus, accessing this front-end directly allows to invoke the
 ** typical lifecycle and management operations (open, close, save, load). Since the
 ** Session plays such a central role, we obviously want to expose just an interface
 ** to client code, both regarding the Session itself, and the session manager.
 ** 
 ** The SessManagerImpl involves the LifecylceAdvisor, which holds all the logic to
 ** manage start-up and shutdown of the session, including starting of the core services
 ** and opening of the external facade interfaces.
 ** 
 ** @todo as of 2016, this effort is considered stalled but basically valid
 */


#ifndef MOBJECT_SESSION_SESS_MANAGER_IMPL_H
#define MOBJECT_SESSION_SESS_MANAGER_IMPL_H

#include "steam/mobject/session/session-impl.hpp"
#include "lib/sync.hpp"


namespace steam {
namespace mobject {
namespace session {
  
  using lib::Sync;
  using lib::RecursiveLock_NoWait;
  
  class LifecycleAdvisor;
  
  
  /**
   * Session manager implementation class holding the
   * actual smart pointer to the current Session impl.
   */
  class SessManagerImpl
    : public SessManager
    , public Sync<RecursiveLock_NoWait>
    {
      unique_ptr<SessionImplAPI>   pSess_;
      unique_ptr<LifecycleAdvisor> lifecycle_;
      
      SessManagerImpl();
      friend class lib::DependencyFactory<SessManagerImpl>;
      
     ~SessManagerImpl() ;
      
      /* ==== SessManager API ==== */
      virtual void clear()        override;
      virtual void close()        override;
      virtual void reset()        override;
      virtual void load ()        override;
      virtual void save (string)  override;
      
      virtual bool isUp ()        override;
      
      
    public:
      /* ==== steam layer internal API ==== */
      
      virtual SessionImplAPI* operator-> ()  noexcept override;
      
    };
  
  
  
}}} // namespace steam::mobject::session
#endif
