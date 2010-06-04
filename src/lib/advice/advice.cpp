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
#include "lib/singleton.hpp"
#include "include/logging.h"

#include <boost/noncopyable.hpp>

using lib::Singleton;

namespace lib {
namespace advice {
  
//  LUMIERA_ERROR_DEFINE (MISSING_INSTANCE, "Existing ID registration without associated instance");
  
  namespace { // ======= implementation of the AdviceSystem ============
    
    class AdviceSystem
      : public Index<PointOfAdvice>
      , boost::noncopyable
      {
        
      public:
        AdviceSystem()
          {
            INFO (library, "Initialising Advice Index tables.");
          }
        
       ~AdviceSystem()
          {
            INFO (library, "Shutting down Advice system.");
          }
      };
    
    
    /** hidden implementation-level access to the AdviceSystem */
    Singleton<AdviceSystem> aSys;
    
    
  } //(End) AdviceSystem implementation

  
  
  
  
  /*  ====== AdviceLink : access point for Provisions and Requests ====== */  
  
  
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
  
  
  /** when the Provision actually sets advice data, this is copied
   *  into an internal buffer within the AdviceSystem. We then use the
   *  Index to remember the presence of this advice data and to detect
   *  possible matches with existing advice::Request entries.
   *  @param adviceData pointer to the copied data,
   *         actually pointing to an ActiveProvision<AD>
   */
  void
  AdviceLink::publishProvision (PointOfAdvice* newProvision)
  {
    const PointOfAdvice* previousProvision (getSolution (*this));
    setSolution (this, newProvision);
    
    if (!previousProvision && newProvision)
      aSys().addProvision (*newProvision);
    else
    if (previousProvision && newProvision)
      aSys().modifyProvision (*previousProvision, *newProvision);
    else
    if (previousProvision && !newProvision)
      aSys().removeProvision (*previousProvision);  ////////////////////////////TODO: don't we need to release buffer storage here?
  }
  
  
  /** when advice is retracted explicitly,
   *  after removing the provision index entry
   *  we also need to re-process any requests
   *  which happen to match our binding... 
   */
  void
  AdviceLink::discardSolutions ()
  {
    const PointOfAdvice* existingProvision (getSolution (*this));
    setSolution (this, NULL );
    if (existingProvision)
      aSys().removeProvision (*existingProvision);  ////////////////////////////TODO: don't we need to release buffer storage here?
  }
  
  
  void
  AdviceLink::publishRequestBindingChange(HashVal previous_bindingKey)
  {
    aSys().modifyRequest(previous_bindingKey, *this);
  }
  
  
  void
  AdviceLink::registerRequest()
  {
    aSys().addRequest (*this);
  }
  
  
  void
  AdviceLink::deregisterRequest()
  {
    aSys().removeRequest (*this);
  }


  
  
  
}} // namespace lib::advice
