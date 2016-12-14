/*
  SESS-MANAGER-IMPL.hpp  -  global session access and lifecycle

  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>

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


#ifndef MOBJECT_SESSION_SESS_MANAGER_IMPL_H
#define MOBJECT_SESSION_SESS_MANAGER_IMPL_H

#include "proc/mobject/session/session-impl.hpp"
#include "lib/sync.hpp"


namespace proc {
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
      scoped_ptr<SessionImplAPI>   pSess_;
      scoped_ptr<LifecycleAdvisor> lifecycle_;
      
      SessManagerImpl()  throw();
      friend class lib::DependencyFactory;
      
     ~SessManagerImpl() ;
      
      /* ==== SessManager API ==== */
      virtual void clear()  override;
      virtual void close()  override;
      virtual void reset()  override;
      virtual void load ()  override;
      virtual void save ()  override;
      
      virtual bool isUp ()  override;
      
      
    public:
      /* ==== proc layer internal API ==== */
      
      virtual SessionImplAPI* operator-> ()  noexcept override;
      
    };
  
  
  
}}} // namespace proc::mobject::session
#endif
