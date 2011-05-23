/*
  PlayService  -  interface: render- and playback control

  Copyright (C)         Lumiera.org
    2011,               Hermann Vosseler <Ichthyostega@web.de>

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

* *****************************************************/


#include "proc/play/play-service.hpp"
#include "lib/singleton.hpp"


#include <string>
//#include <memory>
//#include <tr1/functional>
//#include <boost/scoped_ptr.hpp>



namespace proc  {
  namespace play{
  
    using std::string;
    using lumiera::Subsys;
    using std::auto_ptr;
    using boost::scoped_ptr;
    using std::tr1::bind;
    
    
    namespace { // hidden local details of the service implementation....
    
      /** details of how the DummyPlayer service can be started
       *  and used as independent "subsystem" within main()  */
      class DummyPlayerSubsysDescriptor
        : public Subsys
        {
          operator string ()  const { return "Dummy-Player"; }
          
          
          bool 
          shouldStart (lumiera::Option&)
            {
              return false; // for now the DummyPlayerService only comes "up" as dependency,
            }              //  but doesn't start as a subsystem on it's own.
          
          bool
          start (lumiera::Option&, Subsys::SigTerm terminationHandle)
            {
              ASSERT (!thePlayer_);
              
              thePlayer_.reset (new DummyPlayerService (terminationHandle));
              return true;
            }
          
          /** manages the actual (single) instance of the player service impl */
          scoped_ptr<DummyPlayerService> thePlayer_;
          
          
          void
          triggerShutdown ()  throw()
            {
              thePlayer_.reset(0);
              // note: shutdown of the DummyPlayerService instance may block
             //        for a short period, until termination of all tick services
            }
          
          bool 
          checkRunningState ()  throw()
            {
              return (thePlayer_);
            }
        };
      
      lib::Singleton<DummyPlayerSubsysDescriptor> theDummyPlayerDescriptor;
      
      
      
      
      
      /* ================== define an lumieraorg_DummyPlayer instance ======================= */
      
      
    } // (End) hidden service impl details
    
    
    
    
    /** */
  
  
}} // namespace proc::play
