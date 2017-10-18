/*
  UI-COORD-RESOLVER.hpp  -  resolve UI coordinate spec against actual window topology

  Copyright (C)         Lumiera.org
    2017,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file ui-coord-resolver.hpp
 ** Evaluation of UI coordinates against a concrete window topology.
 ** [UI-Coordinates](\ref UICoord) allow to describe and locate an interface component within the
 ** Lumiera GUI through a topological access path. As such these coordinate specifications are abstract,
 ** and need to be related, attached or resolved against the actual configuration of widgets in the UI.
 ** Through this relation it becomes possible to pose meaningful queries over these coordinates, or to
 ** build, rebuild and remould a coordinate specification.
 ** 
 ** We need to avoid tainting with the intrinsics of the actual UI toolkit though -- which indicates
 ** the UICoordResolver should be designed as an abstract intermediary, built on top of a command and
 ** query interface, provided by the \ref Navigator and backed by the actual UI configuration.
 ** 
 ** @todo WIP 10/2017 first draft ////////////////////////////////////////////////////////////////////////////TICKET #1106 generic UI coordinate system
 ** 
 ** @see UICoordResolver_test
 ** @see UICoord_test
 ** @see navigator.hpp
 ** @see view-locator.hpp
 */


#ifndef GUI_INTERACT_UI_COORD_RESOLVER_H
#define GUI_INTERACT_UI_COORD_RESOLVER_H

#include "lib/error.hpp"
#include "lib/symbol.hpp"
#include "gui/interact/ui-coord.hpp"
//#include "lib/util.hpp"

//#include <boost/noncopyable.hpp>
//#include <string>
//#include <vector>
#include <utility>
//#include <memory>


namespace gui {
namespace interact {
  
  namespace error = lumiera::error;
  
//  using std::unique_ptr;
//  using std::string;
//  using lib::Literal;
  using lib::Symbol;
//  using util::unConst;
//  using util::isnil;
//  using util::min;
  

  
  
  /**
   * Interface to discover a backing structure for the purpose of
   * path navigation and resolution.
   */
  class LocationQuery
    {
    public:
      virtual ~LocationQuery() { }  ///< this is an interface
      
      /** */
    };
  
  
  
  
  /**
   * Query and mutate UICoord specifications in relation to actual UI topology.
   * 
   * @todo initial draft as of 10/2017
   */
  class UICoordResolver
    : public UICoord::Builder
    {
      
    public:
      UICoordResolver (UICoord const& uic, LocationQuery& queryAPI)
        : Builder{uic}
      { }
      
      
      /* === query functions === */
      
      /** */
      bool
      isAnchored()  const
        {
          UNIMPLEMENTED ("path anchorage check");
        }

      /** */
      bool
      canAnchor()  const
        {
          UNIMPLEMENTED ("determine if a mutation is possible to anchor the path explicitly");
        }

      /** */
      bool
      isCovered()  const
        {
          UNIMPLEMENTED ("path coverage check");
        }
      
      /** */
      bool
      canCover()  const
        {
          UNIMPLEMENTED ("determine if a mutation is possible to get the path covered");
        }
      
      
      
      /* === mutation functions === */
      
      /**
       */
      UICoordResolver
      cover()
        {
          UNIMPLEMENTED ("mutate the path to get it covered");
          return std::move (*this);
        }
      
      
      /**
       */
      UICoordResolver
      extend (Literal pathExtension)
        {
          UNIMPLEMENTED ("mutate the path to extend it while keeping it partially covered");
          return std::move (*this);
        }
      
      
    private:
    };
  
  
  
}}// namespace gui::interact
#endif /*GUI_INTERACT_UI_COORD_RESOLVER_H*/
