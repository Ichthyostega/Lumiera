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
 ** Attachment point to the UI-Bos.
 ** Every gui::model::Tangible holds a BusTerm, which is linked
 ** to the Tangible's identity, and serves to relay interface actions
 ** towards the Proc-Layer. Moreover, the BusTerm is the service point
 ** to receive structural change messages.
 ** 
 ** @todo as of 11/2015 this is complete WIP-WIP-WIP
 ** 
 ** @see ////TODO_test usage example
 ** 
 */


#ifndef GUI_CTRL_BUS_TERM_H_
#define GUI_CTRL_BUS_TERM_H_


#include "lib/error.hpp"
//#include "lib/symbol.hpp"
//#include "lib/util.hpp"

#include <boost/noncopyable.hpp>
#include <string>


namespace gui {
namespace ctrl{
  
//  using lib::HashVal;
//  using util::isnil;
  using std::string;
  
  
  /**
   * @todo write type comment...
   */
  class BusTerm
    : boost::noncopyable
    {
      string nothing_;
      
    public:
      BusTerm();
     ~BusTerm();
      
    protected:
      string mayday ()  const;
    };
  
  
  
}} // namespace gui::ctrl
#endif /*GUI_CTRL_BUS_TERM_H_*/
