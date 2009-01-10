/*
  SUBSYS.hpp  -  interface for describing an application part to be handled by main() 
 
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

/** @file subsys.hpp
 ** Describing dependencies and lifecycle of the application's primary parts.
 ** Within Lumiera's main(), a small number of well-known application subsystems
 ** need to be started and maintained, observing some interrelations.
 ** While the activation of the key components is controlled by options, maybe
 ** some prerequisite subsystems need to be pulled up, and in case of an regular
 ** or irregular exit of a given subsystem, the whole dependency graph needs 
 ** to be brought down in a clean manner. The purpose of lumiera::Subsys is
 ** to maintain these in a self-explanatory script-like fashion within main(),
 ** without forcing the individual subsystems into a fixed implementation scheme.
 ** The only requirement is that for each subsystem there is sort-of an entry
 ** point or facade object, providing a Subsys descriptor instance to be
 ** used within main. 
 **
 ** @see lumiera::AppState
 ** @see lumiera::Option
 ** @see main.cpp
 */


#ifndef LUMIERA_SUBSYS_H
#define LUMIERA_SUBSYS_H

#include "lib/error.hpp"
#include "common/option.hpp"

#include <boost/noncopyable.hpp>
#include <tr1/functional>
#include <string>
#include <vector>



namespace lumiera {
  
  using std::string;
  using boost::noncopyable;
  using std::tr1::function;
  
  
  
  /**
   * Dependencies and lifecycle of a partially independent Subsystem of the Application.
   * Using such descriptors, AppState as activated from main() is able to pull up,
   * maintain and shut down the primary parts of the Application.
   * @note synchronisation is up to the implementor.
   */
  class Subsys
    : private noncopyable
    {
    public:
      typedef function<void(string*)> SigTerm;
      
      virtual ~Subsys();
      
      /** a human readable name */
      virtual operator string ()  const =0;

      
      /** define a dependency to another Subsys
       *  required for running this subsystem */
      Subsys& depends (Subsys& prereq);
      
      /** @return true if Up
       *  @warning must not block nor throw. */
      bool isRunning();
      
      
      /** query application option state to determine 
       *  if this subsystem should be activated. */
      virtual bool shouldStart (lumiera::Option&)  =0;
      
      
      /** how to start up this subsystem. Failure to start up
       *  usually terminates the whole application. When this subsystem
       *  ceases to work, it must assure to activate the given signal.
       *  @param options may be influencing the operation mode 
       *  @param termination to be signalled by the subsystem.
       *  @warning termination must be signalled reliably.  
       *  @return \c true if actually started. */
      virtual bool start (lumiera::Option&, SigTerm)  =0;
      
      
      /** initiate termination of this subsystem.
       *  may be called repeatedly any time...
       *  @warning must not block nor throw. */
      virtual void triggerShutdown ()  throw() =0;
      
      
      const std::vector<Subsys*>
      getPrerequisites () { return prereq_; }
      
      
    private:
      /** weather this subsystem is actually operational.
       *  When returning \c false here, the application may 
       *  terminate at any point without further notice
       *  Note further, that a subsystem must not be in
       *  running state when signalling termination. */
      virtual bool checkRunningState()  throw() =0;
      
      std::vector<Subsys*> prereq_;
    };
  
  
  
} // namespace lumiera

#endif
