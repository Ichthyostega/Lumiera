/*
  SessManagerImpl  -  global session access and lifecycle
 
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


/** @file sess-manager-impl.cpp
 ** Implementation of the Session management functions.
 ** The Class SessManager is declared alongside with mobject::Session,
 ** because it serves as smart ptr-to-Impl at the same time. Effectively,
 ** the session manager owns the current session object and only grants
 ** access via his overloaded operator->() . Because there is no operator*(),
 ** no one can get at the address of the current session object. (correct?)
 **
 ** @see session-impl.hpp
 ** @see mobject::Session#current
 ** @see mobject::session::SessionManager_test
 **
 */


#include "proc/mobject/session.hpp"
#include "proc/mobject/session/sess-manager-impl.hpp"
#include "proc/mobject/session/defsmanager.hpp"
#include "proc/mobject/session/lifecycle-advisor.hpp"
#include "proc/asset/timeline.hpp"
#include "lib/error.hpp"
#include "lib/query.hpp"

using boost::scoped_ptr;




namespace mobject {
namespace session {
  
  LUMIERA_ERROR_DEFINE (CREATE_SESSION, "unable to create basic session");
  
  /** Access to the "current session", which actually is
   *  an SessionImpl instance. This session object is created 
   *  either by loading an existing session, or on demand by
   *  this accessor function here (when no session was loaded
   *  or created)
   *  @note any exceptions arising while building the basic
   *        session object(s) will halt the system.
   */
  SessionImplAPI*
  SessManagerImpl::operator-> ()  throw()
  {
    if (!pImpl_)
      try
        { // create empty default configured session
          this->reset();
        }
      catch (...)
        {
          ERROR (progress, "Unrecoverable Failure while creating the empty default session.");
          throw lumiera::error::Fatal ( "Failure while creating the basic session object. System halted."
                                      , LUMIERA_ERROR_CREATE_SESSION );
        }
    
    
    return pImpl_.get();
  }
  
  
  
  
  
  
  
  namespace { // defining details of the Session Lifecycle
    
    typedef scoped_ptr<SessionImplAPI> SessionPImpl;
    
    class SessionLifecycleDetails
      : public LifecycleAdvisor
      {
        SessionPImpl & session_;
        bool shall_load_;         ////////////TODO a placeholder; later we'll pass in a de-serialiser
        
        
        
        /** @note Any session services get up into default configured state.
         *        After the swap, \c tmpS holds onto the old session, which
         *        consequently should unwind on leaving this scope. */
        void
        createSessionFacilities()
          {
            SessionPImpl tmpS (new SessionImplAPI);
            session_.swap (tmpS);
          }
        
        
        void
        injectSessionContent()
          {
            if (shall_load_)
              {
                UNIMPLEMENTED ("loading session from persistent storage");
              }
            else
              { // inject some default session content
                asset::PTimeline initialTimeline = session_->defaults (lumiera::Query<asset::Timeline> ());
                /////////////TODO howto "attach a timeline"??
                //session_->attach( timeline );
              }
          }
        
        
        void
        getSessionReady()
          {
            UNIMPLEMENTED ("any wiring and configuration to get the session ready");
          }
        
        
        void
        openSessionInterface()
          {
            UNIMPLEMENTED ("open layer separation interface");
          }
        
        
        void
        closeSessionInterface()
          {
            UNIMPLEMENTED ("shut down layer separation interface");
          }
        
        
        void
        disconnectRenderProcesses()
          {
            UNIMPLEMENTED ("halt rendering");
          }
        
        
        void
        commandLogCheckpoint()
          {
            UNIMPLEMENTED ("tag command log");
          }
        
        
        void
        deconfigure()
          {
            TODO ("reset the assets registered with AssetManager");
            /////////////////////////////////////////////////////////////////// TICKET #154
          }
        
        
        
        
      public:
        SessionLifecycleDetails(SessionPImpl& currentSessionAccessPoint)
          : session_(currentSessionAccessPoint)
          , shall_load_(false)
          { }
      };
    
  }//(End) details of Session Lifecycle
  
  
  
  /** Starting up the session access and lifecycle management.
   *  Initially (at static init time), only the single system-wide
   *  Session manger instance is created. It can be used to load an
   *  existing session; otherwise an empty default Session, together
   *  with the core facilities (PlacementIndex, AssetManager, Query
   *  subsystem and the Defaults manager) is created on first
   *  \link #operator-> access \endlink to the session object.
   */
  SessManagerImpl::SessManagerImpl ()  throw()
    : pImpl_ (0)
    , lifecycle_(new SessionLifecycleDetails(pImpl_))
  {
    Session::initFlag = true;  //////////////////////////////////////// TICKET #518   instead of this hack, implement basic-init of the session manager for real
  }
  
  
  SessManagerImpl::~SessManagerImpl ()
  {
    TODO ("verify sane lifecycle");
    Session::initFlag = false;
  }
  
  
  
  bool
  SessManagerImpl::isUp ()
  {
    return bool(pImpl_);                          ///////////////////// TICKET #702 possible race, because this gets true way before the interface is up
  }
  
  /** @note no transactional behaviour. may succeed partially.
   *  @todo clarify relation to command processing/undo     /////////// TICKET #697
   */
  void
  SessManagerImpl::clear ()
  {
    Lock sync(this);
    pImpl_->clear();
  }
  
  
  /** Shut down the current session together with all associated services.
   *  @todo avoid blocking when aborting render processes ///////////// TICKET #201
   *  @todo well defined transactional behaviour  ///////////////////// TICKET #698
   */
  void
  SessManagerImpl::close ()
  {
    Lock sync(this);
    lifecycle_->shutDown();
    pImpl_.reset();
  }
  
  
  /** @note this operation is atomic and either succeeds or
   *        fails completely, in which case the current session
   *        remains unaltered.
   *  @todo for this to work, we need to change the implementation of
   *        AssetManager so support this kind of transactional switch!
   */
  void
  SessManagerImpl::reset ()
  {
    Lock sync(this);
    lifecycle_->shutDown();
    lifecycle_->pullUp();
  }
  
  
  void
  SessManagerImpl::load ()
  {
    UNIMPLEMENTED ("load serialised session");
    Lock sync(this);
    lifecycle_->shutDown();
    lifecycle_->pullUp();
  }
  
  
  /** \par Implementation details
   *  We intend to have several switchable object serialisers.
   *  One of these serialisers should generate a comprehensible
   *  text based representation suitable for checking into 
   *  SCM systems.
   *  Sessions can be saved into one single file or be split
   *  to several files (master file and edl files)
   */
  void
  SessManagerImpl::save ()
  {
    UNIMPLEMENTED ("save session (serialised)");
    /////////////////////////////////////////////////TODO: need lock?
  }
  
  
  
}} // namespace mobject::session
