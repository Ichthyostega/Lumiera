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
//#include "proc/mobject/session/defsregistry.hpp"
#include "lib/error.hpp"

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
  
  
  
  /** Initially (at static init time), only the single system-wide
   *  Session manger instance is created. It can be used to load an
   *  existing session; otherwise an empty default Session an a
   *  Defaults manager (Config Query system) is created at first
   *  \link #operator-> access \endlink to the session object.
   */
  SessManagerImpl::SessManagerImpl ()  throw()
    : pImpl_ (0)
  { }
  
  
  
  bool
  SessManagerImpl::isUp ()
  {
    return bool(pImpl_);
  }
  
  /** @note no transactional behaviour.
   *        may succeed partial.
   */
  void
  SessManagerImpl::clear ()
  {
    pImpl_->clear();
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
    scoped_ptr<SessionImplAPI> tmpS (new SessionImplAPI);
    
    TODO ("reset the assets registered with AssetManager");
    /////////////////////////////////////////////////////////////////// TICKET #154
    
    TODO ("thread lock");
    pImpl_.swap (tmpS);
  }
  
  
  void
  SessManagerImpl::load ()
  {
    UNIMPLEMENTED ("load serialised session");
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
  }
  
  
  
}} // namespace mobject::session
