/*
  DIAGNOSTIC-OUTPUT-SLOT.hpp  -  helper for testing against the OutputSlot interface

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

/** @file diagnostic-output-slot.hpp
 ** An facility for writing unit-tests against the OutputSlot interface.
 **
 ** @see output-slot-protocol-test.cpp
 */


#ifndef PROC_PLAY_DIAGNOSTIC_OUTPUT_SLOT_H
#define PROC_PLAY_DIAGNOSTIC_OUTPUT_SLOT_H


#include "lib/error.hpp"
#include "proc/play/output-slot.hpp"
#include "lib/iter-source.hpp"  ////////////TODO really going down that path...?
#include "proc/engine/testframe.hpp"
//#include "lib/sync.hpp"

//#include <boost/noncopyable.hpp>
//#include <string>
//#include <vector>
//#include <tr1/memory>
//#include <boost/scoped_ptr.hpp>


namespace proc {
namespace play {

//using std::string;
  using ::engine::test::TestFrame;

//using std::vector;
//using std::tr1::shared_ptr;
//using boost::scoped_ptr;
  
  
  
  /********************************************************************
   * Helper for unit tests: Mock output sink.
   * 
   * @todo write type comment
   */
  class DiagnosticOutputSlot
    : public OutputSlot
    {
    public:
      /** build a new Diagnostic Output Slot instance,
       *  discard the existing one. Use the static query API
       *  for investigating collected data. */
      static OutputSlot&
      build()
        {
          UNIMPLEMENTED ("Diagnostic Output Slot instance");
        }
      
      static DiagnosticOutputSlot&
      access (OutputSlot& to_investigate)
        {
          UNIMPLEMENTED ("access the diagnostics data for the given OutputSlot instance");
        }
      
      
      /* === diagnostics API === */
      
      /**
       * diagnostic facility to verify
       * test data frames written to this
       * Test/Dummy "output"
       */
      struct OutputStreamProtocol
        : lib::IterSource<TestFrame> 
        {
          /////////////TODO: implement the extension points required to drive an IterSource
        };
        
      typedef OutputStreamProtocol::iterator OutFrames;
      
      
      OutFrames
      getChannel (uint channel)
        {
          UNIMPLEMENTED ("access output stream tracing entry");
        }
      
      
      bool
      buffer_was_used (uint channel, FrameID frame)
        {
          UNIMPLEMENTED ("determine if the denoted buffer was indeed used");
        }
      
      
      bool
      buffer_unused   (uint channel, FrameID frame)
        {
          UNIMPLEMENTED ("determine if the specified buffer was never touched/locked for use");
        }
      
      
      bool
      buffer_was_closed (uint channel, FrameID frame)
        {
          UNIMPLEMENTED ("determine if the specified buffer was indeed closed properly");
        }
      
      
      bool
      emitted (uint channel, FrameID frame)
        {
          UNIMPLEMENTED ("determine if the specivied buffer was indeed handed over for emitting output");
        }
      
      
      
    private:
      
    };
  
  
  
}} // namespace proc::play
#endif
