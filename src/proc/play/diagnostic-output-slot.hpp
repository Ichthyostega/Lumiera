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
//#include "lib/sync.hpp"

//#include <boost/noncopyable.hpp>
//#include <string>
//#include <vector>
//#include <tr1/memory>
//#include <boost/scoped_ptr.hpp>


namespace proc {
namespace play {

//using std::string;

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
      
    private:
      
    };
  
  
  
}} // namespace proc::play
#endif
