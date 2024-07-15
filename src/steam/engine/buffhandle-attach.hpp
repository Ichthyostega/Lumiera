/*
  BUFFHANDLE-ATTACH.hpp  -  Buffer handle extension to attach objects into the buffer

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file buffhandle-attach.hpp
 ** Extension to allow placing objects right into the buffers, taking ownership.
 ** This extension is mostly helpful for writing unit-tests, and beyond that for the
 ** rather unusual case where we need to place an full-blown object into the buffer,
 ** instead of just plain data. A possible use case for this mechanism is to allow for
 ** state per calculation stream, feeding this local state to the individual render node
 ** embedded into a "state frame". Some effect processors indeed need to maintain state
 ** beyond the single frame (e.g. averaging, integrating, sound compression), which usually
 ** is handled by applying an "instance" of that processor to the frames to be calculated
 ** in a straight sequence.
 ** 
 ** BuffHandle and the underlying BufferProvider standard implementation support that case
 ** by attaching an object managing functor to the metadata. This way, the state can live
 ** directly embedded into the frame and still be accessed like an object. To keep the
 ** header and compilation footprint low, the implementation of the functions supporting
 ** this special case was split out of the basic buffhandle.hpp
 ** 
 ** @see BuffHandle
 ** @see BufferProviderProtocol_test usage demonstration
 */

#ifndef ENGINE_BUFFHANDLE_ATTACH_H
#define ENGINE_BUFFHANDLE_ATTACH_H


#include "lib/error.hpp"
#include "steam/engine/buffer-provider.hpp"
#include "steam/engine/buffhandle.hpp"


namespace steam {
namespace engine {
  
  
  
  
  /* === BuffHandle Implementation === */
  
  
#define _EXCEPTION_SAFE_INVOKE(_CTOR_)  \
    try                                  \
      {                                   \
        return *new(pBuffer_) _CTOR_;     \
      }                                   \
    catch(...)                            \
      {                                   \
        emergencyCleanup(); /* EX_FREE */ \
        pBuffer_ = 0;                     \
        throw;                            \
      }
  
  /** convenience shortcut: place and maintain an object within the buffer.
   *  This operation performs the necessary steps to attach an object;
   *  if the buffer isn't locked yet, it will do so. Moreover, the created
   *  object will be owned by the buffer management facilities, i.e. the
   *  destructor is registered as cleanup function.
   * @throw error::Logic in case there is already another TypeHandler registered
   *        in charge of managing the buffer contents, or when the object to create
   *        would not fit into this buffer.
   */
  template<typename BU>
  inline BU&
  BuffHandle::create()
  {
    takeOwnershipFor<BU>();
    _EXCEPTION_SAFE_INVOKE (BU());
  }
  
#undef _EXCEPTION_SAFE_INVOKE
  
  
  
  /** @internal helper to attach an TypeHandler after-the fact.
   *  @note this prepares the buffer for placement-creating an embedded object.
   *        It doesn't actually create an object
   * @throw error::Logic in case there is already another TypeHandler registered
   *        in charge of managing the buffer contents, or when the object to create
   *        would not fit into this buffer.
   */
  template<typename BU>
  inline void
  BuffHandle::takeOwnershipFor()
  {
    BuffDescr howto_attach_object_automatically
      = descriptor_.provider_->getDescriptor<BU>();
    takeOwnershipFor (howto_attach_object_automatically); // EX_STRONG
  }
  
  
  /** convenience shortcut: access the buffer contents casted to a specific type.
   * @warning this is a \em blind cast, there is no type safety.
   * @note clients can utilise the metadata::LocalKey to keep track of some
   *       specific property of the buffer, like e.g. the type of object.
   */
  template<typename BU>
  inline BU&
  BuffHandle::accessAs()
  {
    if (!pBuffer_)
      throw error::Logic ("buffer not (yet) locked for access by clients"
                         , LERR_(LIFECYCLE));
    return *reinterpret_cast<BU*> (pBuffer_);
  }
  
  
  
}} // namespace steam::engine
#endif
