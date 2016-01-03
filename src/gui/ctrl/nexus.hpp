/*
  NEXUS.hpp  -  UI-Bus central hub and routing table

  Copyright (C)         Lumiera.org
    2015,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file nexus.hpp
 ** Core hub and routing table of the UI-Bus.
 ** Any relevant element within the Lumiera GTK UI is connected to the [UI-Bus](ui-bus.hpp)
 ** through some [bus terminal](bus-term.hpp). Actually, there is one special BustTerm
 ** implementation, which acts as router and messaging hub.
 ** 
 ** @todo initial draft and WIP-WIP-WIP as of 11/2015
 ** 
 ** @see TODO_abstract-tangible-test.cpp
 ** 
 */


#ifndef GUI_CTRL_NEXUS_H
#define GUI_CTRL_NEXUS_H


#include "lib/error.hpp"
//#include "lib/idi/entry-id.hpp"
#include "lib/idi/genfunc.hpp"
#include "gui/ctrl/bus-term.hpp"
#include "gui/model/tangible.hpp"
//#include "lib/util.hpp"
//#include "gui/model/tangible.hpp"
//#include "lib/diff/record.hpp"
#include "lib/idi/entry-id.hpp"

#include <boost/noncopyable.hpp>
//#include <string>
#include <unordered_map>


namespace gui {
namespace ctrl{
  
//  using lib::HashVal;
//  using util::isnil;
//  using lib::idi::EntryID;
//  using lib::diff::Record;
//  using std::string;
  
  
  /**
   * Central hub of the UI-Bus.
   * This special implementation of the [BusTerm] interface maintains
   * a routing table and manages the connections to individual UI-Elements.
   * The nexus relies on a dedicated (up)link to the [CoreService] to handle
   * command invocation and presentation state. This is implemented by wiring
   * the aforementioned CoreService terminal as "up-link", while actually
   * defining special handling overrides for _all other kinds of messages_.
   * The purpose of the Nexus is to route all these _other kinds of messages._
   * 
   * @todo write type comment
   */
  class Nexus
    : public BusTerm
    , boost::noncopyable
    {
      typedef std::unordered_map<EntryID, Tangible*, EntryID::UseEmbeddedHash> RoutingTable;
      
      RoutingTable routingTable_;
      
      
    protected:
      /** route mark messages down to the individual Tangible.
       * @note only messages to elements currently registered
       *       in the routing table are dispatched. All other
       *       messages are dropped silently.
       */
      virtual void
      mark (ID subject, GenNode const& mark)  override
        {
          auto entry = routingTable_.find (subject);
          if (entry == routingTable_.end())
            return;
          else
            entry->second->mark (mark);
        }
      
      /** add a new down-link connection to the routing table
       * @param identity the [endpoint-ID](\ref BusTerm::endpointID_) used
       *         to address the new element to be connected to the bus.
       * @param newNode to add the address (!) into the routing table
       * @return backlink for the new Tangible's BusTerm to
       *         attach itself to the Nexus.
       * @note at call time, the second param, the newNode will typically
       *         be just a Tangible (and not a subclass yet),
       *         since this function is invoked from ctor.
       */
      virtual BusTerm&
      routeAdd (ID identity, Tangible& newNode)  override
        {
          routingTable_[identity] = &newNode;
          return *this;
        }
      
      
      /** deactivate and remove a down-link route.
       * @note will be invoked by the dtor of the node's BusTerm.
       */
      virtual void
      routeDetach (ID node)  noexcept override
        {
          routingTable_.erase (node);
        }
      
      
      virtual operator string()  const
        {
          return lib::idi::instanceTypeID(this);
        }
      
      
      size_t
      size()  const
        {
          return routingTable_.size();
        }
      
    public:
      explicit
      Nexus (BusTerm& uplink_to_CoreService, ID identity =lib::idi::EntryID<Nexus>())
        : BusTerm(identity, uplink_to_CoreService)
      { }
    };
  
  
  
}} // namespace gui::ctrl
#endif /*GUI_CTRL_NEXUS_H*/
