/*
  Wizard  -  controller user help and assistance

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

* *****************************************************/


/** @file wizard.cpp
 ** Implementation of the global help controller to show the user manual.
 ** 
 ** @todo WIP 3/2017 early draft of the UI top-level controllers
 */


//#include "gui/gtk-lumiera.hpp"
#include "gui/interact/wizard.hpp"
#include "gui/interact/spot-locator.hpp"
#include "gui/ctrl/global-ctx.hpp"
//#include "lib/util.hpp"

//using util::cStr;
//using util::isnil;


namespace gui {
namespace interact {
  
  using ctrl::GlobalCtx;
  
  
  // dtors via smart-ptr invoked from here... (TODO any to come here?)
  Wizard::~Wizard()
    { }
  
  
  Wizard::Wizard (GlobalCtx& globals)
    : globalCtx_{globals}
    { }
  
  
  /** */
  
  
  
}}// namespace gui::interact
