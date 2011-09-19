/*
  DiagnosticBufferProvider  -  helper for testing against the BufferProvider interface

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

* *****************************************************/


#include "lib/error.hpp"
#include "include/logging.h"
#include "lib/meta/function.hpp"
#include "lib/scoped-ptrvect.hpp"

#include "proc/engine/diagnostic-buffer-provider.hpp"

#include <boost/scoped_array.hpp>
//#include <vector>

using lib::ScopedPtrVect;
using boost::scoped_array;



namespace engine {
  
  
  /** Storage for the diagnostics frontend */
  lib::Singleton<DiagnosticBufferProvider> DiagnosticBufferProvider::diagnostics;
  
  
  class Block
    : boost::noncopyable
    {
      size_t size_;
      scoped_array<char> storage_;
      
      bool was_locked_;
      
    public:
      Block()
        : size_(0)
        , storage_()
        , was_locked_(false)
        { }
      
      bool
      was_used()  const
        {
          return was_locked_;
        }
    };
  
    
    
  namespace { // Details of allocation and accounting

    using lumiera::typelist::FunctionSignature;
    using std::tr1::placeholders::_1;
    using std::tr1::function;
    using std::tr1::bind;
    
    /**
     * type rebinding template for issuing flexible queries
     * and forwarding them to the individual block entries
     */
    template<typename FUN>
    struct _Query
      {
        typedef typename FunctionSignature<FUN>::Ret Result;
      };
    
    function<bool(Block&)> QUERY_was_used = bind (&Block::was_used, _1 );
  
  } // (END) Details of allocation and accounting
  
  
  
  /**
   * @internal DiagnosticBufferProvider's PImpl.
   * Uses a linearly growing table of heap allocated buffer blocks,
   * which will never be discarded, unless the PImpl is discarded as a whole.
   * This way, the tracked usage information remains available after the fact.
   */
  class DiagnosticBufferProvider::HeapMemProvider
    : public BufferProvider
    , public ScopedPtrVect<Block>
    {
      virtual BuffHandle
      lockBufferFor (BufferDescriptor const& descriptor)
        {
          UNIMPLEMENTED ("lock buffer for exclusive use");
        }
      
      virtual void
      releaseBuffer (BuffHandle const& handle)
        {
          UNIMPLEMENTED ("release a buffer and invalidate the handle");
        }
      
    public:
      HeapMemProvider()
        {
        }
      
      virtual ~HeapMemProvider()
        {
          INFO (proc_mem, "discarding %uz diagnostic buffer entries", HeapMemProvider::size());
        }
      
      template<typename FUN>
      typename _Query<FUN>::Result
      accessStats (uint bufferID, FUN queryFunction)
        {
          queryFunction (access_or_create (bufferID));
        }
      
    private:
      Block&
      access_or_create (uint bufferID)
        {
          UNIMPLEMENTED ("either get existing block or create new entries to fit");
        }
    };
  
  
  
  BufferProvider&
  DiagnosticBufferProvider::build()
  {
    return diagnostics().reset();
  }
  
  
  DiagnosticBufferProvider&
  DiagnosticBufferProvider::access (BufferProvider const& provider)
  {
    if (!diagnostics().isCurrent (provider))
      throw error::Invalid("given Provider doesn't match (current) diagnostic data record."
                           "This might be an lifecycle error. Did you build() this instance beforehand?");
    
    return diagnostics();
  }
  

    
    
  DiagnosticBufferProvider::HeapMemProvider&
  DiagnosticBufferProvider::reset()
  {
    pImpl_.reset(new HeapMemProvider());
    return *pImpl_;
  }
  
  bool
  DiagnosticBufferProvider::isCurrent (BufferProvider const& implInstance)
  {
    return &implInstance == pImpl_.get();
  }

  

  
  
  /* === diagnostic API === */
  
  bool
  DiagnosticBufferProvider::buffer_was_used (uint bufferID)  const
    {
      pImpl_->accessStats (bufferID, QUERY_was_used);
    }
  
  
  bool
  DiagnosticBufferProvider::buffer_was_closed (uint bufferID)  const
    {
      UNIMPLEMENTED ("check closed-flag of a specific buffer");
    }
  
  
  void*
  DiagnosticBufferProvider::accessStorage (uint bufferID)  const
    {
      
    }
  

} // namespace engine
