/*
  SESSION-IMPL.hpp  -  holds the complete session data to be edited by the user
 
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
 
*/


/** @file session-impl.hpp
 ** Session and SessionManager Implementation classes.
 ** Session and the corresponding Manager are primary Interfaces
 ** to control the behaviour of the editing part of the application.
 ** All all implementation complexities are hidden behind a "PImpl".
 **
 ** This file contains the implementation classes, it should never
 ** be included by client code.
 ** 
 ** @see session-service-access-test.cpp for a complete simplified mock session manager 
 **
 */


#ifndef MOBJECT_SESSION_SESSIONIMPL_H
#define MOBJECT_SESSION_SESSIONIMPL_H

#include "proc/mobject/session.hpp"
#include "proc/mobject/session/edl.hpp"
#include "proc/mobject/session/fixture.hpp"
#include "proc/mobject/session/placement-index.hpp"

#include <boost/scoped_ptr.hpp>
#include <vector>




namespace mobject {
namespace session {
  
  using std::vector;
  using boost::scoped_ptr;
  using std::tr1::shared_ptr;

  
  /**
   * Implementation class for the Session interface
   */
  class SessionImpl : public mobject::Session
    {
      uint focusEDL_;
      vector<EDL> edls;
      PFix fixture;
      shared_ptr<PlacementIndex> pIdx_;
      
      /* ==== Session API ==== */
      virtual bool isValid ();
      virtual void add (PMO& placement);
      virtual bool remove (PMO& placement);
      
      virtual EDL&  currEDL ();
      
      virtual PFix& getFixture ();
      virtual void rebuildFixture ();
      
    protected: /* == management API === */
      SessionImpl (DefsManager&)  throw();
      friend class SessManagerImpl;
      
      void clear ();
      
    };
  
  
  /**
   * Session manager implementation class holding the
   * actual smart pointer to the current Session impl.
   * @todo couldn't this be pushed down into session.cpp?
   */
  class SessManagerImpl : public SessManager
    {
      scoped_ptr<DefsManager> pDefs_;
      scoped_ptr<SessionImpl> pImpl_;
      
      SessManagerImpl()  throw();
      friend class lib::singleton::StaticCreate<SessManagerImpl>;
      
      virtual ~SessManagerImpl() {}
      
      /* ==== SessManager API ==== */
      virtual void clear () ;
      virtual void reset () ;
      virtual void load ()  ;
      virtual void save ()  ;
      virtual SessionImpl* operator-> ()  throw() ;
      
      
    public:
      /* ==== proc layer internal API ==== */
      
      /** @internal access point for PlacementIndex and PlacementRef */
      static shared_ptr<PlacementIndex>& getCurrentIndex () ;  
      
    };
  
  
  
}} // namespace mobject::session
#endif
