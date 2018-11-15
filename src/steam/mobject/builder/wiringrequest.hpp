/*
  WIRINGREQUEST.hpp  -  (interface) the intention to make a data or control connection

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file wiringrequest.hpp
 ** Symbolic representation of a goal for node wiring.
 ** A WiringRequest is discovered while walking the Session model, and will be resolved
 ** later, when the target for the desired connection is available. When all wiring requests
 ** are resolved, the build process finishes. Thus, creating and resolving of such requests
 ** is the "fuel" actually to drive the Builder.
 ** @todo stalled design draft from 2008 -- basically still considered relevant as of 2016
 */


#ifndef MOBJECT_BUILDER_WIRINGREQUEST_H
#define MOBJECT_BUILDER_WIRINGREQUEST_H



namespace proc {
namespace mobject {
namespace builder {
  
  
  /**
   * A statefull value object denoting the wish to establish a link or connection
   * between two entities. Used to organise the proper working of the build process.
   * Wiring requests are first to be checked and can be deemed impossible to 
   * satisfy. Internally, wiring requests contain specific information about
   * the objects to be connected. This information is exposed only to the
   * ConManager, which is the facility actually wiring the connections.
   */
  class WiringRequest
    {
    public:
      /**
       * TODO design sketch......
       */
    };



}}} // namespace proc::mobject::builder
#endif
