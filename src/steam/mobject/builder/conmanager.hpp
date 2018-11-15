/*
  CONMANAGER.hpp  -  manages the creation of data/control connections when building the Renderengine

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


/** @file conmanager.hpp
 ** @todo initially generated code of unclear relevance
 */


#ifndef STEAM_MOBJECT_BUILDER_CONMANAGER_H
#define STEAM_MOBJECT_BUILDER_CONMANAGER_H



namespace steam    {
namespace mobject {
namespace builder {
  
  
  /**
   * Connection Manager: used to build the connections between render engine nodes
   * if these nodes need to cooperate besides the normal "data pull" operation. 
   * Esp. the Connection Manager knows how to wire up the effect's parameters 
   * with the corresponding ParamProviders (automation) in the Session.
   * Questions regarding the possibility of a media stream connection are 
   * delegated internally to the STypeManager.
   * 
   * The primary service of the connection manager is to accept a wiring request
   * and handle the details of establishing the necessary connections.
   */
  class ConManager
    {
    public:
      /**
       * @todo just a design sketch from 2008, work out signatures etc...
       */
      int getConnection ();
    };



}}} // namespace steam::mobject::builder
#endif
