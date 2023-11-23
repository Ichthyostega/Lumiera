/*
  LAZY-INIT.hpp  -  a self-initialising functor

  Copyright (C)         Lumiera.org
    2023,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file lazy-init.hpp
 ** Building block to allow delayed initialisation of infrastructure tied to a functor.
 ** This solution is packaged as a mix-in template and engages a hidden mechanism with
 ** considerable trickery. It attempts to solve a problem arising notoriously when building
 ** elaborate processing by composing functions and user-provided configuration lambdas;
 ** the very point of this construction style is to tap into internal context to involve
 ** deep details of the implementation without the need to represent these as structures
 ** on API level. Unfortunately this has the consequence that capture-by-reference is
 ** all over the place, breeding instability. The only solution to defeat this instability
 ** is to lock an enclosing implementation scope into a fixed memory location, which boils
 ** down to using non-copyable classes. This solution may be in conflict to the intended
 ** use, especially when building DSLs, configuration frameworks or symbolic processing,
 ** where entities are _value like_ from a semantic point of view. The solution pursued
 ** here is to define some linkage for operational state, which allows to lock a scope
 ** to a fixed memory location. Assuming that a typical usage scenario will first
 ** require setup, then proceed to processing, this solution attempts to tie
 ** the usage restrictions to the lifecycle — hopefully hiding the concern
 ** from users sight altogether.
 ** 
 ** # Initialisation mechanism
 ** This mix-in assumes that there is a function somewhere, which activates the actual
 ** processing, and this processing requires initialisation to be performed reliably
 ** before first use. Thus, a _»trojan functor«_ is placed into this work-function,
 ** with the goal to activate a „trap“ on first use. This allows to invoke the actual
 ** initialisation, which is also configured as a functor, and which is the only part
 ** the client must provide actively, to activate the mechanism.
 ** 
 ** There is one _gory detail_ however: the initialisation hook needs the actual instance
 ** pointer valid *at the time of actual initialisation*. And since initialisation shall
 ** be performed automatically, the trap mechanism needs a way to derive this location,
 ** relying on minimal knowledge only. This challenge can only be overcome by assuming
 ** that the »trojan functor« itself is stored somehow embedded into the target object
 ** to be initialised. If there is a fixed distance relation in memory, then the target
 ** can be derived from the self-position of the functor; if this assumption is broken
 ** however, memory corruption and SEGFAULT may be caused. 
 ** 
 ** @todo 11/2023 at the moment I am just desperately trying to get a bye-product of my
 **       main effort into usable shape and salvage an design idea that sounded clever
 **       on first thought. I am fully aware that »lazy initialisation« is something
 **       much more generic, but I am also aware of the potential of the solution
 **       coded here. Thus I'll claim that generic component name, assuming that
 **       time will tell if we need a more generic framework to serve this
 **       purpose eventually....
 ** @see LazyInit_test
 ** @see lib::RandomDraw usage example
 ** @see vault::gear::TestChainLoad::Rule where this setup matters
 */


#ifndef LIB_LAZY_INIT_H
#define LIB_LAZY_INIT_H


//#include "lib/error.h"
//#include "lib/nocopy.hpp"
#include "lib/meta/function.hpp"
//#include "lib/meta/function-closure.hpp"
//#include "lib/util-quant.hpp"
//#include "lib/util.hpp"

//#include <functional>
//#include <utility>


namespace lib {
//  namespace err = lumiera::error;
  
//  using lib::meta::_Fun;
//  using std::function;
//  using std::forward;
//  using std::move;
  
  /**
   * 
   */
  class LazyInit
    {
      
    };
  
  
  
} // namespace lib
#endif /*LIB_LAZY_INIT_H*/
