/*
  STATE-MAP-GROUPING-STORAGE.hpp  -  grouping storage to track presentation state

  Copyright (C)         Lumiera.org
    2016,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file state-map-grouping-storage.hpp
 ** Implementation of storage for captured presentation state.
 ** This is a associative storage, grouped by element ID.
 ** 
 ** @todo as of 2/2016 this is complete WIP-WIP-WIP
 ** 
 ** @see ////TODO_test usage example
 ** 
 */


#ifndef GUI_INTERACT_STATE_MAP_GROUPING_STORAGE_H
#define GUI_INTERACT_STATE_MAP_GROUPING_STORAGE_H


#include "lib/error.hpp"
//#include "gui/ctrl/bus-term.hpp"
//#include "lib/idi/entry-id.hpp"
#include "lib/diff/gen-node.hpp"
//#include "lib/symbol.hpp"
//#include "lib/util.hpp"

#include <boost/noncopyable.hpp>
#include <string>


namespace gui {
namespace interact {
  
//  using lib::HashVal;
//  using util::isnil;
  using std::string;
  
  
  /**
   * Map storage for captured presentation state information.
   * @todo write type comment...
   */
  class StateMapGroupingStorage
    : boost::noncopyable
    {
    protected:
      
    public:
      /** retrieve captured state
       * @return reference to the state mark last seen for the denoted property
       *         or reference to a generic "no" marker (Ref::NO)
       */
      lib::diff::GenNode const&
      retrieve (string elementSymbol, string propertyID)  const
        {
          UNIMPLEMENTED("retrieve captured state");
        }
      
    private:
    };
  
  
  
}} // namespace gui::interact
#endif /*GUI_INTERACT_STATE_MAP_GROUPING_STORAGE_H*/
