/*
  DummyPlayerService  -  access point and service implementing a dummy test player
 
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
 
* *****************************************************/


#include "proc/dummy-player-service.hpp"
#include "lib/singleton.hpp"

#include <string>


namespace proc {

  using std::string;
  using lumiera::Subsys;
  
  class DummyPlayerSubsysDescriptor
    : public Subsys
    {
      operator string ()  const { return "Engine"; }
      
      bool 
      shouldStart (lumiera::Option&)
        {
          TODO ("determine, if renderengine should be started");
          return false;
        }
      
      bool
      start (lumiera::Option&, Subsys::SigTerm termination)
        {
          UNIMPLEMENTED ("pull up renderengine and register shutdown hook");
          return false;
        }
      
      void
      triggerShutdown ()  throw()
        {
          UNIMPLEMENTED ("initiate halting the engine");
        }
      
      bool 
      checkRunningState ()  throw()
        {
          //Lock guard (*this);
          TODO ("implement detecting running state");
          return false;
        }
    };
  
  namespace {
    lumiera::Singleton<DummyPlayerSubsysDescriptor> theDescriptor;
  }
  
  
  
  
  /** @internal intended for use by main(). */
  Subsys&
  DummyPlayer::getDescriptor()
  {
    return theDescriptor();
  }



} // namespace proc
