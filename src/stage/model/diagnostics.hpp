/*
  DIAGNOSTICS.hpp  -  helper for verifying the GUI-session connection

  Copyright (C)         Lumiera.org
    2014,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file stage/model/diagnostics.hpp
 ** Service for diagnostics.
 ** This header defines the basics of...
 ** 
 ** @note as of X/2014 this is complete bs
 ** @todo WIP  ///////////////////////TICKET #
 ** 
 ** @see ////TODO_test usage example
 ** @see diagnostics.cpp implementation
 ** 
 */


#ifndef STAGE_MODEL_DIAGNOSTICS_H
#define STAGE_MODEL_DIAGNOSTICS_H


#include "lib/error.hpp"
#include "lib/nocopy.hpp"
//#include "lib/symbol.hpp"
//#include "lib/util.hpp"

//#include <string>


  
namespace stage {
namespace model {
  
//  using util::isnil;
//  using std::string;
  
  enum ProbeMode {
    TEST_SESSION_1
  };
  
  /**
   * Basic (abstracted) view of...
   * 
   * @see SomeSystem
   * @see NA_test
   */
  class Diagnostics
    : util::NonCopyable
    {
      
    public:
      
      
      explicit
      Diagnostics (ProbeMode m)
        { }
      
     ~Diagnostics()
        {
          
        }
      
      
      /* == Adapter interface for == */
      
      void
      setSolution (string const& solution ="")
        {
          UNIMPLEMENTED ("tbw");
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #888
          if (isDeaf())
            this->transmogrify (solution);
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #888
        }
      
      
    protected:
      void maybe ()  const;
      
      
    };
  
  
  
  
  
  
  /** @internal in case
   */
  inline void
  Diagnostics::maybe ()  const
  {
    UNIMPLEMENTED ("tbw");
  }
  
  
  
  
  
}} // namespace stage::model
#endif /*STAGE_MODEL_DIAGNOSTICS_H*/
