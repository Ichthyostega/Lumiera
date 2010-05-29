/*
  Advice  -  generic loosely coupled interaction guided by symbolic pattern
 
  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>
 
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


#include "lib/advice.hpp"
#include "lib/advice/index.hpp"


namespace lib {
namespace advice {
  
//  LUMIERA_ERROR_DEFINE (MISSING_INSTANCE, "Existing ID registration without associated instance");

  
  /** allocate raw storage for a buffer holding the actual piece of advice.
      We need to manage this internally, as the original advice::Provision
      may go out of scope, while the advice information as such remains valid.
      @note the special twist is the size of the buffer depending on the actual
            advice type, which we need to erase for tracking all advice provisions
            and advice requests through an generic index datastructure.
      @todo rewrite to use Lumiera's block allocator / memory pool */
  void*
  AdviceLink::getBuffer(size_t)
  {
    UNIMPLEMENTED ("raw allocation and de-allocation of advice holding buffer");
  }
  
  
  void
  AdviceLink::publishProvision (PointOfAdvice*)
  {
    UNIMPLEMENTED ("change advice provision registration");
  }
  
  
  void
  AdviceLink::discardSolutions ()
  {
    UNIMPLEMENTED ("notify index of retracted advice");
  }
  
  
  void
  AdviceLink::publishBindingChange ()
  {
    UNIMPLEMENTED ("propagate binding change to index");
  }
      
      
  void
  AdviceLink::publishRequestBindingChange()
  {
    UNIMPLEMENTED ("propagate binding change to index");
  }
  
  
  void
  AdviceLink::registerRequest()
  {
    UNIMPLEMENTED ("registrate request with the index");
  }
  
  
  void
  AdviceLink::deregisterRequest()
  {
    UNIMPLEMENTED ("detach request from index");
  }


  
  
  
}} // namespace lib::advice
