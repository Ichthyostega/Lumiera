/*
  MOCKCONFIGRULES.hpp  -  mock implementation of the config rules system
 
  Copyright (C)         CinelerraCV
    2007,               Hermann Vosseler <Ichthyostega@web.de>
 
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


/** @file mockconfigrules.hpp
 ** Mock/Test/Debugging Implementation of the config rules system.
 ** Instead of actually parsing/analyzing/resolving queries, this implementation
 ** uses a Table of hard wired queries together with preconfigured object instances
 ** as answer values. As of 1/2008 it is used to "keep the implementation work going"
 ** -- later on, when we use a real Prolog interpreter, it still may be useful for
 ** testing and debugging.
 **
 ** @see cinelerra::Query
 ** @see cinelerra::ConfigRules
 **
 */


#ifndef CINELERRA_MOCKCONFIGRULES_H
#define CINELERRA_MOCKCONFIGRULES_H

#include "common/configrules.hpp"

#include <string>



namespace cinelerra
  {
  using std::string;
  
  
  namespace query
    {
    using mobject::session::Track;
    using asset::Port;
    
    /** 
     * Dummy Implementation of the query interface.
     * Provides an explicit implementation using hard wired
     * values for some types of interest for testing and debugging.
     */
    class MockConfigRules : public cinelerra::ConfigRules
      {
      protected:
        MockConfigRules ();
        friend class cinelerra::singleton::StaticCreate<MockConfigRules>;
        
      public:
        
        virtual Track resolve (Query<Track> q);
        virtual Port  resolve (Query<Port> q);
      };
    
    
  
  } // namespace query
    
} // namespace cinelerra
#endif
