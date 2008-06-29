/*
  STATE.hpp  -  Key Interface representing a render process and encapsulating state
 
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


#ifndef PROC_INTERFACE_STATE_H
#define PROC_INTERFACE_STATE_H


#include "pre.hpp"

#include "proc/lumiera.hpp"

#include <cstddef>


namespace engine { 
  
  class StateAdapter;
  
  class BuffHandle
    {
    protected:
      typedef float Buff;
      typedef Buff* PBuff;//////TODO define the Buffer type(s)
      
      PBuff pBuffer_; 
      long sourceID_;
      
      BuffHandle (PBuff pb, long id)
        : pBuffer_(pb),
          sourceID_(id)
        { }
      
    public:
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
    };
  
  
  class State
    {
    protected:
      virtual ~State() {};
      
      /** resolves to the State object currently "in charge".
       *  Intended as a performance shortcut to avoid calling
       *  up through a chain of virtual functions when deep down
       *  in chained ProcNode::pull() calls. This allows derived
       *  classes to proxy the state inteface.
       */ 
      virtual State& getCurrentImplementation () =0;
      
      friend class engine::StateAdapter;
      
    public:
      
    };
  
} // namespace engine



namespace proc_interface {

  using engine::State;
    
    
} // namespace proc_interface
#endif
