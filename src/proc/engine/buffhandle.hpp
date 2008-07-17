/*
  BUFFHANDLE.hpp  -  Buffer handling support for the render engine
 
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

/** @file buffhandle.hpp
 ** Various bits needed to support the buffer management within the render nodes.
 ** When pulling data from predecessor nodes and calculating new data, each render node
 ** needs several input and output buffers. These may be allocated and provided by several
 ** different "buffer providers" (for example the frame cache). For accessing those buffers,
 ** the node needs to keep a table of buffer pointers, and for releasing the buffers later
 ** on, we need some handles. The usage pattern of those buffer pointer tables is stack-like,
 ** thus it makes sense to utilize a single large buffer pointer array per pull() calldown
 ** sequence and dynamically claim small chunks for each node.
 **
 ** @see nodeoperation.hpp
 ** @see bufftable.hpp       storage for the buffer table
 ** @see engine::Invocation
 */

#ifndef ENGINE_BUFFHANDLE_H
#define ENGINE_BUFFHANDLE_H


#include "common/error.hpp"


namespace engine {
  
  
  
  /**
   * Handle for a buffer for processing data, abstracting away the actual implementation.
   * The real buffer pointer can be retrieved by dereferencing this smart-handle class.
   */
  struct BuffHandle
    {
      typedef float Buff;
      typedef Buff* PBuff;//////TODO define the Buffer type(s)
      
      PBuff 
      operator->() const 
        { 
          return pBuffer_; 
        }
      Buff&
      operator* () const
        {
          ENSURE (pBuffer_);
          return *pBuffer_;
        }
      
    protected:
      PBuff pBuffer_; 
      long sourceID_;
    };
  
  
  /**
   * Buffer Type information.
   * Given a BufferDescriptor, it is possible to allocate a buffer
   * of suitable size and type by using State::allocateBuffer().
   */
  struct BufferDescriptor
    {
      char typeID_; ///////TODO define the Buffer type(s)
    };
  
  
  
} // namespace engine
#endif
