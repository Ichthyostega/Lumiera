/*
  OUTPUT-SLOT.hpp  -  capability to transfer data to a physical output

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

*/

/** @file output-slot.hpp
 ** An (abstract) capability to send media data to an external output.
 ** OutputSlot is the central metaphor for the organisation of actual (system level) outputs;
 ** using this concept allows to separate and abstract the data calculation and the organisation
 ** of playback and rendering from the specifics of the actual output sink. Actual output
 ** possibilities can be added and removed dynamically from various components (backend, GUI),
 ** all using the same resolution and mapping mechanisms 
 **
 ** @see output-test-slot.hpp  ////TODO
 */


#ifndef PROC_PLAY_OUTPUT_SLOT_H
#define PROC_PLAY_OUTPUT_SLOT_H


#include "lib/error.hpp"
#include "lib/handle.hpp"
#include "lib/time/timevalue.hpp"
#include "proc/engine/buffhandle.hpp"
#include "lib/iter-source.hpp"
//#include "lib/sync.hpp"

#include <boost/noncopyable.hpp>
//#include <string>
//#include <vector>
//#include <tr1/memory>
//#include <boost/scoped_ptr.hpp>


namespace proc {
namespace play {

  using ::engine::BuffHandle;
  using lib::time::Time;
//using std::string;

//using std::vector;
//using std::tr1::shared_ptr;
//using boost::scoped_ptr;
  
  
  /** established output channel */
  class Connection;
  
  class BufferHandoverSink
    : public lib::Handle<Connection>
    {
      
    public:
      void emit(Time, BuffHandle);
    };
  
  
  class SharedBufferSink
    : public lib::Handle<Connection>
    {
      
    public:
      void emit(Time);
    };
  
  
  
  
  /********************************************************************
   * Interface: Generic output sink.
   * 
   * @todo write type comment
   */
  class OutputSlot
    : boost::noncopyable
    {
    public:
      virtual ~OutputSlot();
      
      typedef lib::IterSource<BufferHandoverSink>::iterator Opened_BufferHandoverSinks;
      typedef lib::IterSource<SharedBufferSink>::iterator   Opened_SharedBufferSinks;
      
      template<class SINK>
      struct Allocation
        {
          typedef typename lib::IterSource<SINK>::iterator OpenedSinks;
          
          OpenedSinks getOpenedSinks();
          
          bool isActive();
          
          /////TODO add here the getters for timing constraints
        };
      
      
      bool isFree()  const;
      
      Allocation<BufferHandoverSink>
      allocate();
      
    private:
      
    };
  
  
  
}} // namespace proc::play
#endif
