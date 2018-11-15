/*
  TESTROOT.hpp  -  test dummy model root for checking Model/Session functionality

  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file testroot.hpp
 ** Setup of a faked session root for unit testing.
 ** This works together with some likewise manipulated mock rules to build content
 ** with known layout to be scrutinised in test
 */


#ifndef MOBJECT_SESSION_TESTROOT_H
#define MOBJECT_SESSION_TESTROOT_H


#include "steam/mobject/session/label.hpp"
#include "steam/mobject/session/mobjectfactory.hpp"
#include "lib/symbol.hpp"

using lib::Symbol;


namespace steam {
namespace mobject {
namespace session {
namespace test    {
  
  inline Placement<Label>
  make_dummyRoot()
  {
    Symbol labelType ("dummyRoot");
    return MObject::create (labelType);
  }
  
  
  
  
  
}}}} // namespace steam::mobject::session::test
#endif
