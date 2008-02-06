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

#include <boost/any.hpp>
#include <string>



namespace cinelerra
  {
  using std::string;
  
  
  namespace query
    {
    using asset::ProcPatt;
    using asset::PProcPatt;
    
    using boost::any;
    using boost::any_cast;
    
    
    /** a traits-class to define the smart-ptr to wrap the result */
    template<class TY>
    struct WrapReturn                   { typedef shared_ptr<TY> Wrapper;  };
    
    template<>
    struct WrapReturn<const ProcPatt>   { typedef PProcPatt Wrapper;  };
    
    
    /** 
     * the actual table holding preconfigured answers
     * packaged as boost::any objects. 
     */
    class MockTable : public cinelerra::ConfigRules
      {
      protected:
        MockTable ();
        const any& fetch_from_table_for (const string& queryStr);
      };
    
    
    /** 
     * building block defining how to do 
     * the mock implementation for \i one type.
     * We simply access a table holding pre-created objects.
     */
    template<class TY, class BASE>
    class LookupPreconfigured : public BASE
      {
        typedef typename WrapReturn<TY>::Wrapper Ret;
        
      public:
        virtual Ret 
        resolve (const Query<TY>& q)
          {
            TODO ("handle mismatch and not-found case");
            return any_cast<Ret> (fetch_from_table_for (q));
          }
      };
    
    
    /** 
     * Dummy Implementation of the query interface.
     * Provides an explicit implementation using hard wired
     * values for some types of interest for testing and debugging.
     */
    class MockConfigRules 
      : public typelist::InstantiateChained < InterfaceTypes           
                                            , LookupPreconfigured  // building block used for each of the types
                                            , MockTable           //  for implementing the base class (interface) 
                                            >
      {
      protected:
        MockConfigRules ();                                   ///< to be used only by the singleton factory
        friend class cinelerra::singleton::StaticCreate<MockConfigRules>;

        virtual ~MockConfigRules() {}
        
      public:
        
        // TODO: implementation of any additional functions on the ConfigRules inteface goes here
      };
      
    
    
  
  } // namespace query
    
} // namespace cinelerra
#endif
