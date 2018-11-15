/*
  TYPED-ID.hpp  -  registration service for ID to type association

  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file typed-id.hpp
 ** Frontend for a registration service to associate object identities, symbolic identifiers and types.
 ** For maintaining persistent objects, generally an unique object ID is desirable. Within Lumiera, we employ
 ** 128 hash-IDs \c LUID. But hash-IDs arn't self explanatory for human readers and expressive when used for
 ** testing, configuration and rules. A symbolic ID reads much better for this purpose. This registration
 ** service is used to bridge this gap, allowing for symbolic lookup of objects with known type and for
 ** enumeration of all objects of a specific type.
 ** 
 ** @note as of 4/2010 this is an experimental setup and implemented just enough to work out
 **       the interfaces. Ichthyo expects this registration service to play a crucial role
 **       when it comes to integrating a real resolution engine (prolog). For now, the
 **       only interesting point is how to handle the automatic registration.
 ** @todo and as of 2016, we acknowledge the importance of this approach,
 **       just the topic is deferred for now and will be picked up later...
 ** 
 ** @see configrules.hpp
 ** @see typed-lookup.cpp corresponding implementation
 ** @see typed-id-test.cpp
 ** 
 */


#ifndef ASSET_TYPED_ID_H
#define ASSET_TYPED_ID_H


#include "lib/error.hpp"
//#include "steam/asset.hpp"
//#include "steam/asset/struct-scheme.hpp"
//#include "lib/hash-indexed.hpp"
//#include "lib/util.hpp"
#include "lib/symbol.hpp"

//#include <boost/operators.hpp>
#include <memory>

namespace lumiera{  ///////TODO: shouldn't that be namespace lib? or steam?
namespace query  {
  
  /**
   * Adapter for using this type as a \em primary type
   * within Lumiera's Steam-Layer. Any such type is expected
   * to provide a specialisation, filling in the actual details
   * necessary to deal with objects of this type.
   * 
   * @todo design draft as of 4/2010.
   *       This interface should eventually be merged with
   *       the TypeHandler defined in configrules.hpp
   */
  template<class TY>
  class TypeHandlerXX;
  
}}

namespace steam {
namespace asset {
  
//  using std::string;
//  using std::ostream;
    using lib::Symbol;
  
//  using lumiera::error::LUMIERA_ERROR_WRONG_TYPE;
  
  LUMIERA_ERROR_DECLARE (MISSING_INSTANCE);  ///< Existing ID registration without associated instance
  
  namespace {
    
  }
  
  
  
  /**
   */
  class TypedID
    {
      
    public:
      
      template<typename TY>
      class Link;
    };
  
  
  /**
   * Mixin for automatic TypedID registration
   * 
   * @todo unfinished design; especially: have to care for correct copying ////////////////////TICKET #593 
   */
  template<typename TY>
  class TypedID::Link
    {
    protected:
      Link (Symbol id =0)
        {
          TODO ("actually do a registration with TypedID");
        }
      
     ~Link ()
        {
          TODO ("clean up any TypedID registration for this instance");
        }
      
    public:
      void
      registerInstance (std::weak_ptr<TY> smart_ptr_instance)
        {
          TODO ("actually attach the smart-ptr to the existing TypedID registration");
        }
    };
  
  
  
}} // namespace steam::asset
#endif
