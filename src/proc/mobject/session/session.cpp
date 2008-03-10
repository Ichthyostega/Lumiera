/*
  Session  -  holds the complete session to be edited by the user
 
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


/** @file session.cpp
 ** Actual connection between the Session interface and its Implementation.
 ** Holds the storage for the SessionManager implementation (singleton)
 ** 
 ** @see session::SessionImpl
 ** @see session::SessionManagerImpl
 **
 */


#include "proc/mobject/session.hpp"
#include "proc/mobject/session/sessionimpl.hpp"
#include "proc/mobject/session/defsmanager.hpp"

#include "common/singleton.hpp"


using lumiera::Singleton;
using mobject::session::SessManager;
using mobject::session::SessManagerImpl;

namespace mobject
  {

  /** the sole acces point for all client code to the system-wide
   *  "current session". Implemented as smart pointer to singleton
   *  implementation object, where the smart pointer is actually
   *  the SessionManager (which is singleton as well...).
   * 
   *  Consequently, if you want to talk to the <i>session manager,</i>
   *  you use dot-notation, while you access the <i>session object</i>
   *  via arrow notaion (e.g. \code Session::current->getFixture() )
   */
  SessManager& Session::current = Singleton<SessManagerImpl>()();

    
    
    
  Session::~Session () 
  { }

  Session::Session (session::DefsManager& def)  throw()
    : defaults(def)
  { }



} // namespace mobject
