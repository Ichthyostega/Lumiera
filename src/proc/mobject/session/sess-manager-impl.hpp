/*
  SESS-MANAGER-IMPL.hpp  -  global session access and lifecycle
 
  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>
 
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
 
*/


#ifndef MOBJECT_SESSION_SESS_MANAGER_IMPL_H
#define MOBJECT_SESSION_SESS_MANAGER_IMPL_H

#include "proc/mobject/session/session-impl.hpp"


namespace mobject {
namespace session {
  
  class LifecycleAdvisor;
  
  
  /**
   * Session manager implementation class holding the
   * actual smart pointer to the current Session impl.
   */
  class SessManagerImpl : public SessManager
    {
      scoped_ptr<SessionImplAPI>   pImpl_;
      scoped_ptr<LifecycleAdvisor> lifecycle_;
      
      SessManagerImpl()  throw();
      friend class lib::singleton::StaticCreate<SessManagerImpl>;
      
     ~SessManagerImpl() ;
      
      /* ==== SessManager API ==== */
      virtual void clear () ;
      virtual void close () ;
      virtual void reset () ;
      virtual void load ()  ;
      virtual void save ()  ;
      
      virtual bool isUp ()  ;
      
      
    public:
      /* ==== proc layer internal API ==== */
      
      virtual SessionImplAPI* operator-> ()  throw() ;
      
    };
  
  
  
}} // namespace mobject::session
#endif
