/*
  PLUGINADAPTER.hpp  -  Adapter for integrating various Effect processors in the render pipeline

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


/** @file pluginadapter.hpp
 ** TODO pluginadapter.hpp
 */


#ifndef ENGINE_PLUGINADAPTER_H
#define ENGINE_PLUGINADAPTER_H

#include "proc/engine/procnode.hpp"



namespace proc {
namespace engine
  {


  /**
   * Adapter used to integrate an effects processor in the render pipeline.
   * Effects processors are typically defined in a separate library and
   * will be loaded at runtime using Lumiera's plugin interface.
   */
  class PluginAdapter : public ProcNode
    {
      /////////////
    };



}} // namespace proc::engine
#endif
