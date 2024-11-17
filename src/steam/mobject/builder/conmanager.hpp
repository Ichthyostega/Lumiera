/*
  CONMANAGER.hpp  -  manages the creation of data/control connections when building the Renderengine

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

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
