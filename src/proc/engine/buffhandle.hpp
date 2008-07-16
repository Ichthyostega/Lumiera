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

/** @file procnode.hpp
 ** Interface to the processing nodes and the render nodes network.
 **
 ** Actually, there are three different interfaces to consider
 ** - the ProcNode#pull is the invocation interface. It is call-style (easily callable by C)
 ** - the builder interface, comprised by the NodeFactory and the WiringFactory. It's C++ (using templates)
 ** - the actual processing function is supposed to be a C function; it uses a set of C functions 
 **   for accessing the frame buffers with the data to be processed.
 **
 ** By using the builder interface, concrete node and wiring descriptor classes are created,
 ** based on some templates. These concrete classes form the "glue" to tie the node network
 ** together and contain much of the operation beahviour in a hard wired fashion.
 **
 ** @see nodefactory.hpp
 ** @see operationpoint.hpp
 */

#ifndef ENGINE_BUFFHANDLE_H
#define ENGINE_BUFFHANDLE_H


//#include "proc/mobject/parameter.hpp"

//#include <vector>



namespace engine {

//  using std::vector;
  
  
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
  
  
    /** 
     * Tables of buffer handles and corresponding dereferenced buffer pointers.
     * Used within the invocation of a processing node to calculate data.
     * The tables are further differentiated into input data buffers and output
     * data buffers. The tables are supposed to be implemented as bare "C" arrays,
     * thus the array of real buffer pointers can be fed directly to any processing
     * function
     */
  struct BuffTable
    {
      BuffHandle        *const outHandle;
      BuffHandle        *const inHandle;
      BuffHandle::PBuff *const outBuff;
      BuffHandle::PBuff *const inBuff;
    };
  
  
} // namespace engine
#endif
