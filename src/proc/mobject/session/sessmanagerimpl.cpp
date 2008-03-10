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


/** @file sessmanagerimpl.cpp
 ** Implemention of the Session management functions.
 ** The Class SessManager is declared alongside with mobject::Session,
 ** because it serves as smart ptr-to-Impl at the same time. Effectively,
 ** the session manager owns the current session object and only grants
 ** access via his overloaded operator->() . Because there is no operator*(),
 ** no one can get at the address of the current session object. (correct?)
 **
 ** @see sessionimpl.hpp
 ** @see mobject::Session#current
 ** @see mobject::session::SessionManager_test
 **
 */


#include "proc/mobject/session.hpp"
#include "proc/mobject/session/sessionimpl.hpp"

using boost::scoped_ptr;



namespace mobject
  {
  namespace session
    {

    /** Besides creating the single system-wide Session manger instance,
     *  creates an empty default Session as well.
     *  @note any exceptions arising in the course of this will halt
     *        the system (and this behaviour is desirable).
     */
    SessManagerImpl::SessManagerImpl ()  throw()
      : pImpl_ (new SessionImpl)
    {
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
     *        failes completely, in which case the current session
     *        remains unaltered.
     *  @todo for this to work, we need to change the implementation of
     *        AssetManager so support this kind of transactional switch!
     */
    void
    SessManagerImpl::reset ()
    {
      scoped_ptr<SessionImpl> tmp (new SessionImpl);
      
      TODO ("reset the assets registered with AssetManager");
      // Ichthyo-intern: ticket #95
      
      pImpl_.swap (tmp);
    }


    void
    SessManagerImpl::load ()
    {
      UNIMPLEMENTED ("load serialized session");
    }


    /** \par Implementation details
     *  We intend to have several switchable object serialisers.
     *  One of these serializers should genarate a comprehensible
     *  text based representation suitable for checking into 
     *  SCM systems.
     *  Sessions can be saved into one single file or be splitted
     *  to several files (master file and edl files)
     */
    void
    SessManagerImpl::save ()
    {
      UNIMPLEMENTED ("save session (serialized)");
    }



  } // namespace mobject::session

} // namespace mobject
