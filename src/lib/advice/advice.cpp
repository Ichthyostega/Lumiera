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


/** @file advice.cpp 
 ** Implementation the AdviceSystem, to support the advice collaboration.
 ** The AdviceSystem is implemented as singleton, but is never accessed directly
 ** by clients participating in an advice collaboration. Rather, they use the
 ** advice::Request and advice::Provision value classes as a frontend. While
 ** these frontend classes are templated on the concrete advice type, the common
 ** baseclass AdviceLink isn't, allowing the AdviceSystem to operate on type erased
 ** PointOfAdvice entries. The protected access functions within AdviceLink are
 ** implemented in this compilation unit and access the AdviceSystem singleton
 ** defined here locally.
 ** 
 ** \par memory management
 ** Advice data, when added by an advice::Provision, is copied into a ActiveProvision,
 ** which acts as a value holding buffer. This way, the provided advice data is copied
 ** into storage managed by the AdviceSystem, allowing to access the data even after the
 ** original advice::Provision went out of scope.
 ** 
 ** But while the Provision is still alive, it may be used to set new advice, modify the
 ** binding or even retract the given piece of advice. Thus we need a mechanism to link
 ** the ActiveProvision (value holder) to its origin while the latter is still there.
 ** Basically we'll use the PointOfAdvice::resolution_ pointer embedded within advice::Provision
 ** to point to the ActiveProvision entry incorporated into the advice system.
 ** (For advice::Request, the same pointer is used to link to the ActiveProvision yielding
 ** the advice solution, if any). Handling the relation between Provision and ActiveProvision
 ** this way entails some kind of "unofficial" ownership and is slightly incorrect, but seems
 ** the most straight forward implementation. Thus each Provision cares for "his" advice and
 ** just detaches when going away. Consequently, by default, advice provisions remain active
 ** during the lifetime of the application. We'll see if this causes problems.
 ** 
 ** @note when a Provision is copied, this hidden link is not shared with the copy, which
 ** therefore behaves as if newly created with the same binding, but without providing Advice.
 ** 
 ** \par implementing the allocations
 ** The problem with copying and incorporating the ActiveProvision objects is the undetermined
 ** size of these value holders, because the frontend objects are templated on the advice type,
 ** while the AdviceSystem doesn't have any knowledge of the specific advice type. This advice
 ** type is used to set a type guard predicate into each binding, but there is no way to 
 ** re-discover the specifically typed context; the type guards can only be checked for a match.
 ** Thus we need the help of the frontend objects, which need to provide a deleter function
 ** when providing concrete advice data; this deleter function will be saved as function pointer
 ** so to be able to deallocate all advice data when the AdviceSystem is shut down
 **
 ** @todo currently this is unimplemented and we happily leak memory....
 ** @todo rewrite the allocation to use Lumiera's mpool instead of heap allocations
 ** 
 ** \par synchronisation
 ** While the frontend objects are deliberately \em not threadsafe, the lookup implementation
 ** within the AdviceSystem uses a system wide advice::Index table and thus needs locking.
 ** Besides the protection against corrupting the index, this also serves as memory barrier,
 ** so that when a new advice solution is determined and set as a pointer within the matching
 ** requests, this change is actually "committed" from cache to memory. But note, when using
 ** advice::Request concurrently, you need to employ an additional read barrier to ensure
 ** your thread/CPU picks up such newly determined solutions from main memory. Otherwise
 ** you might even try to access superseded and already deleted advice data.
 ** 
 ** @see advice.hpp
 ** @see advice::Index
 **
 */


#include "lib/advice.hpp"
#include "lib/advice/index.hpp"
#include "lib/singleton.hpp"
#include "include/logging.h"

#include <boost/noncopyable.hpp>

using lib::Singleton;

namespace lib {
namespace advice {
  
  
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
            advice type, which type information we need to erase for tracking all
            advice provisions and requests through an generic index datastructure.
      @todo rewrite to use Lumiera's block allocator / memory pool */
  void*
  AdviceLink::getBuffer(size_t siz)
  {
    try { return new char[siz]; }
    
    catch(std::bad_alloc&)
      {
        throw error::Fatal("Unable to store Advice due to memory exhaustion");
      }
  }
  
  
  void
  AdviceLink::releaseBuffer (const void* buff, size_t)
  { 
    delete[] (char*)buff; 
  }
  
  
  
  /** when the Provision actually sets advice data, this is copied
   *  into an internal buffer within the AdviceSystem. We then use the
   *  Index to remember the presence of this advice data and to detect
   *  possible matches with existing advice::Request entries.
   *  @param adviceData pointer to the copied data,
   *         actually pointing to an ActiveProvision<AD>
   *  @return pointer to an superseded old provision entry,
   *          which the caller then needs to de-allocate.
   *          The caller is assumed to know the actual type
   *          and thus the size of the entry to deallocate.
   *          Returning \c NULL in case no old entry exists.
   */
  const PointOfAdvice*
  AdviceLink::publishProvision (PointOfAdvice* newProvision)
  {
    const PointOfAdvice* previousProvision (getSolution());
    this->setSolution (newProvision);
    
    if (!previousProvision && newProvision)
      aSys().addProvision (*newProvision);
    else
    if (previousProvision && newProvision)
      aSys().modifyProvision (*previousProvision, *newProvision);
    else
    if (previousProvision && !newProvision)
      aSys().removeProvision (*previousProvision);
    
    return previousProvision;  // to be deallocated by caller if non-NULL
  }
  
  
  /** when advice is retracted explicitly,
   *  after removing the provision index entry
   *  we also need to re-process any requests
   *  which happen to match our binding... 
   *  @return pointer to the existing provision entry,
   *          to be deallocated by the caller, which
   *          is assumed to know it's exact type.
   */
  const PointOfAdvice*
  AdviceLink::discardSolutions ()
  {
    const PointOfAdvice* existingProvision (getSolution());
    this->setSolution ( NULL );
    if (existingProvision)
      aSys().removeProvision (*existingProvision);
    return existingProvision;
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
