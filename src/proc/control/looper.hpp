/*
  LOOPER.hpp  -  proc dispatcher loop and timing control logic

  Copyright (C)         Lumiera.org
    2016,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file looper.hpp
 ** Implementation building block of ProcDispatcher to control waiting and timing.
 ** This helper encapsulates the loop control logic to separate it from actual
 ** implementation of timing and waiting (per pthread condition variables).
 ** It exposes a combined condition (to be used for waiting) plus any further
 ** controls to manage the operation of the actual queue. The actual tasks to
 ** be controlled are installed as functors.
 ** 
 ** @todo WIP-WIP as of 12/2016 
 ** 
 ** @see DispatcherLooper_test
 ** @see proc-dispatcher.hpp
 ** @see DispatcherLoop
 ** @see CommandQueue
 **
 */



#ifndef PROC_CONTROL_LOOPER_H
#define PROC_CONTROL_LOOPER_H

#include "lib/error.hpp"   ////////TODO needed?
//#include "common/subsys.hpp"
//#include "lib/depend.hpp"

//#include <memory>
//#include <functional>



namespace proc {
namespace control {
  
//  using lib::Symbol;
//  using std::bind;
  
  
  
  /**
   * @todo Type-comment
   */
  class Looper
    {
      
    public:
      Looper()
      { }
      
      
      
      /* == diagnostics == */
      
//    size_t size() const ;
//    bool empty()  const ;
      
    };
  ////////////////TODO 12/16 currently just fleshing  out the API....
  
  
  
  
}} // namespace proc::control
#endif /*PROC_CONTROL_LOOPER_H*/
