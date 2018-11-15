/*
  SOURCE.hpp  -  Representation of a Media source

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


/** @file source.hpp
 ** Processing node to read media data.
 ** The render engine node network is activated by _"pulling"_ from the _exit nodes._
 ** Recursively, any rendering calculations will eventually hit a engine::Source node,
 ** to retrieve existing (raw) media data
 ** @todo this file is there since the very first code generation steps at start
 **       of the Lumiera project. It is not yet clear if those source reading nodes
 **       will indeed be ProcNode subclasses, or rather specifically configured
 **       processing nodes...
 */


#ifndef ENGINE_SOURCE_H
#define ENGINE_SOURCE_H

#include "steam/engine/procnode.hpp"



namespace steam {
namespace engine
  {


  /**
   * Source Node: represents a media source to pull data from.
   * Source is special, because it has no predecessor nodes,
   * but rather makes calls down to the vault layer internally
   * to get at the raw data.
   */
  class Source : public ProcNode
    {
      Source (WiringDescriptor const&);
    };



}} // namespace steam::engine
#endif
