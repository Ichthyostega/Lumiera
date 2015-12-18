/*
  BUS-TERM.hpp  -  connection point for UI elements to the UI-Bus

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


/** @file bus-term.hpp
 ** Attachment point to the UI-Bus.
 ** Every gui::model::Tangible holds a BusTerm, which is linked
 ** to the Tangible's identity, and serves to relay interface actions
 ** towards the Proc-Layer, to remember state changes and to broadcast
 ** notifications. Moreover, the BusTerm is the service point
 ** to receive structural change messages.
 ** 
 ** \par Lifecycle and identity
 ** An BusTerm is always created starting from another BusTerm, to
 ** which it will be wired. Moreover, each BusTerm bears a distinct
 ** [identity][::endpointID_], which is used as _implicit subject_
 ** for emanating messages, or as explicit destination for routing.
 ** The whole [UI-Bus][BusController] is built to perform within the
 ** UI event thread and thus is _not threadsafe_. For that reason,
 ** the automatic detachment built into each BusTerm's dtor is
 ** sufficient to ensure sane connectivity.
 ** 
 ** @todo as of 11/2015 this is complete WIP-WIP-WIP
 ** 
 ** @see ////TODO_test usage example
 ** 
 */


#ifndef GUI_CTRL_BUS_TERM_H
#define GUI_CTRL_BUS_TERM_H


#include "lib/error.hpp"
//#include "lib/symbol.hpp"
//#include "lib/util.hpp"
#include "lib/idi/entry-id.hpp"
#include "lib/diff/gen-node.hpp"

#include <boost/noncopyable.hpp>
#include <utility>
#include <string>


namespace gui {
namespace model {
  class Tangible;
}
namespace ctrl{
  
//  using lib::HashVal;
//  using util::isnil;
  using lib::idi::EntryID;
  using lib::diff::GenNode;
  using std::string;
  
  
  /**
   * connection point at the UI-Bus.
   * The UI-Bus is a star shaped network of terminal points,
   * where each \link Tangible tangible UI element \endlink
   * holds a BusTerm serving access point. The BusTerm interface
   * exposes the basic "verbs" available for communication within
   * the UI
   * - to _act_ on an element means to issue a command
   * - to _note_ some state or information for later replay
   * - to _mark_ as erroneous, send an information message,
   *   replay remembered presentation state or effect structural change
   * There are \em indirect variants of the message verbs, which are
   * intended for routing, broadcasting or forwarding. Effectively,
   * routing is determined from the context and meaning of a message,
   * where the _act_ and _note_ messages have an implicit receiver
   * (either the Proc-Layer or the UI state manager), while the
   * _mark_ messages are always directed _downstream_ towards
   * some element.
   */
  class BusTerm
    {
      using EntryID = lib::idi::BareEntryID;
      using ID = EntryID const&;
      using Tangible = gui::model::Tangible const&;
      
      EntryID endpointID_;
      BusTerm& theBus_;
      
    public:
      virtual ~BusTerm();  ///< this is an interface
      
      virtual void act (GenNode const& command);
      virtual void note (GenNode const& mark);
      
      virtual void note (ID subject, GenNode const& mark);
      virtual void mark (ID subject, GenNode const& mark);
      
      ID getID()  const { return endpointID_; }
      
      
      BusTerm attach (ID, Tangible newNode);
      
      /** may be moved, but not copied,
       *  due to the embedded identity */
      BusTerm(BusTerm&&) = default;
      
    protected:
      BusTerm(ID identity, BusTerm& attached_to)
        : endpointID_(identity)
        , theBus_(attached_to)
        { }
      
      virtual BusTerm& routeAdd(Tangible);
      virtual void routeDetach(ID)  noexcept;
    };
  
  
  
}} // namespace gui::ctrl
#endif /*GUI_CTRL_BUS_TERM_H*/
