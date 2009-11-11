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
 ** This file contains the implementation level API, it should never
 ** be included by client code. Besides the actual SessionImpl, a set
 ** of further implementation level services is provided for use by
 ** Proc-Layer's internals. These additional SessionServices are to be
 ** accessed through dedicated headers and interface classes (typically
 ** through static access functions), thereby abstracting from the actual
 ** session implementation. Within this file, the implementation of these
 ** SessionServices is wired up with the SessionImpl object.
 ** 
 ** @see Session public API
 ** @see session-services.hpp
 ** @see session-service-access-test.cpp for a complete simplified mock session manager 
 **
 */


#ifndef MOBJECT_SESSION_SESSIONIMPL_H
#define MOBJECT_SESSION_SESSIONIMPL_H

#include "proc/mobject/session.hpp"
#include "proc/mobject/session/edl.hpp"
#include "proc/mobject/session/fixture.hpp"
#include "proc/mobject/session/placement-index.hpp"
#include "proc/mobject/session/session-services.hpp"

#include "proc/mobject/session/session-service-fetch.hpp"
#include "proc/mobject/session/session-service-explore-scope.hpp"
#include "proc/mobject/session/session-service-mock-index.hpp"
#include "proc/mobject/session/session-service-defaults.hpp"


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
      
      scoped_ptr<DefsManager> defaultsManager_;   ///////////TODO: later, this will be the real defaults manager. Currently this is just never initialised (11/09)
      
      
      /* ==== Session API ==== */
      virtual bool isValid ();
      virtual void add (PMO& placement);
      virtual bool remove (PMO& placement);
      
      virtual EDL&  currEDL ();
      
      virtual PFix& getFixture ();
      virtual void rebuildFixture ();
      
    protected: /* == management API === */
      SessionImpl ();
      
      void clear ();
      friend class SessManagerImpl;
      
      PPIdx const&
      getPlacementIndex()
        {
          ENSURE (pIdx_);
          return pIdx_;
        }
      
    };
  
  
    /* ===== providing internal services for Proc ===== */
    
    template<class IMPL>
    struct ServiceAccessPoint<SessionServiceFetch, IMPL>
      : IMPL
      {
        bool
        isRegisteredID (PMO::ID const& placementID)
          {
            return IMPL::getPlacementIndex()->contains (placementID); //never throws
          }
        
        PMO&
        resolveID (PMO::ID const& placementID)
          {
            return IMPL::getPlacementIndex()->find (placementID); //may throw
          }
      };
    
    
    template<class IMPL>
    struct ServiceAccessPoint<SessionServiceExploreScope, IMPL>
      : IMPL
      {
        QueryResolver&
        getResolver()
          {
            UNIMPLEMENTED ("how actually to manage the PlacementIndexQueryResolver wrapper instance");
            
//          return IMPL::magic_;
          }
      };
    
    
    template<class IMPL>
    struct ServiceAccessPoint<SessionServiceMockIndex, IMPL>
      : IMPL
      {
        PPIdx const&
        getPlacementIndex()
          {
            if (mockIndex_)
              return mockIndex_;
            else
              return IMPL::getPlacementIndex();
          }
        
        void
        reset_PlacementIndex (PPIdx const& alternativeIndex)
        {
          mockIndex_ = alternativeIndex;
        }
        
      private:
        PPIdx mockIndex_;
      };
    
    
    template<class IMPL>
    struct ServiceAccessPoint<SessionServiceDefaults, IMPL>
      : IMPL
//    , SessionServiceDefaults
      {

        ////////////////////////////TODO
      };
  
    
    
    
    
    class SessManagerImpl;
    
    typedef SessionServices< Types< SessionServiceFetch
                                  , SessionServiceExploreScope
                                  , SessionServiceMockIndex
                                  , SessionServiceDefaults
                                  >              // List of the APIs to provide
                           , SessManagerImpl    //  frontend for access
                           , SessionImpl       //   implementation base class
                           >                  //
                           SessionImplAPI;
  
  
  
  /**
   * Session manager implementation class holding the
   * actual smart pointer to the current Session impl.
   * @todo couldn't this be pushed down into session.cpp?
   */
  class SessManagerImpl : public SessManager
    {
      scoped_ptr<SessionImplAPI> pImpl_;
      
      SessManagerImpl()  throw();
      friend class lib::singleton::StaticCreate<SessManagerImpl>;
      
      virtual ~SessManagerImpl() {}
      
      /* ==== SessManager API ==== */
      virtual void clear () ;
      virtual void reset () ;
      virtual void load ()  ;
      virtual void save ()  ;
      
      
    public:
      /* ==== proc layer internal API ==== */
      
      virtual SessionImplAPI* operator-> ()  throw() ;
      
    };
  
  
  
}} // namespace mobject::session
#endif
