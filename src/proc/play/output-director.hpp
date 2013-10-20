/*
  OUTPUT-DIRECTOR.hpp  -  handling all the real external output connections

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

*/

/** @file output-director.hpp
 ** A global service to coordinate and handle all external output activities.
 ** The OutputDirector is an application internal singleton service for
 ** coordinating and controlling all actual input/output- and rendering
 ** capabilities, exposing distinct lifecycle functions to connect, bring
 ** up and shut down what can be considered the "Player/Output" subsystem. 
 **
 ** @see output-manager-test.cpp  ////TODO
 */


#ifndef PROC_PLAY_OUTPUT_DIRECTOR_H
#define PROC_PLAY_OUTPUT_DIRECTOR_H


#include "lib/error.hpp"
#include "lib/depend.hpp"
#include "proc/play/output-manager.hpp"
#include "common/subsys.hpp"
#include "lib/sync.hpp"

#include <boost/noncopyable.hpp>
//#include <string>
//#include <vector>
//#include <tr1/memory>
#include <boost/scoped_ptr.hpp>


namespace proc {
namespace play {
  
//using std::string;
//using std::vector;
//using std::tr1::shared_ptr;
  using boost::scoped_ptr;
  
  
  class PlayService;
  
  
//typedef lib::ScopedPtrVect<DisplayerSlot> DisplayerTab;
  
  
  
  /******************************************************
   * Management of external Output connections.
   * 
   * @todo write Type comment
   */
  class OutputDirector
    : boost::noncopyable
    , public lib::Sync<>
    {
      typedef lumiera::Subsys::SigTerm SigTerm;
      
      scoped_ptr<PlayService> player_;
      
    public:
      static lib::Depend<OutputDirector> instance;
      
      bool connectUp() ;
      void triggerDisconnect(SigTerm)  throw();
      
      bool isOperational()  const;
      
    private:
      OutputDirector() ;
     ~OutputDirector() ;
      friend class lib::DependencyFactory;
      
      
      void bringDown (SigTerm completedSignal);
    };
  
  
  
}} // namespace proc::play
#endif
