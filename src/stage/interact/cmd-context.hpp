/*
  CMD-CONTEXT.hpp  -  interface to access command context binding services within the UI

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


/** @file cmd-context.hpp
 ** Abstraction: support for binding command invocation into an UI context.
 ** The interact::CmdContext front-end is used when commands need to be bound to a whole context
 ** of UI elements rather than to a specific widget -- especially for picking up command arguments
 ** from interaction state found within that context. To invoke a command, typically it is necessary
 ** to prepare a _command instance_ and to pick up and bind _command arguments._ In most cases, this
 ** can be done directly, using a fixed command-ID and locally known argument values. But in the
 ** aforementioned special situation, we have to deal with ongoing user interactions, which might
 ** span several widgets, where some interaction events just happen to make invocation of a specific
 ** command viable. The prototypical example is when some element is dragged around, and, depending
 ** on the vicinity, might cause quite different actions when dropped. Such behaviour can be achieved
 ** with the help of an intermediary, known as ["interaction state manager"](\ref InteractionState).
 ** The command context accessor allows to discover a specific InteractionState instance, which
 ** - is responsible for the specific command to be invoked
 ** - can handle context information related to a specific _control system_ (e.g mouse, keyboard,
 **   hardware controller, pen)
 ** - might handle changing contextual state and thus decide if a command can be invoked
 ** From the InteractionState instance, it is possible to retrieve a notification when a specific,
 ** context-bound command becomes executable by picking up suitable parameter values from this context.
 ** 
 ** @todo as of 3/2017 this is a early design draft and WIP-WIP-WIP
 ** @todo after initial design draft, postponed as of 10/2017
 ** 
 ** @see CmdContext_test
 ** @see ////TODO_test usage example
 ** 
 */


#ifndef STAGE_INTERACT_CMD_CONTEXT_H
#define STAGE_INTERACT_CMD_CONTEXT_H


#include "lib/error.hpp"
#include "lib/nocopy.hpp"
//#include "stage/ctrl/bus-term.hpp"
//#include "lib/idi/entry-id.hpp"
#include "lib/hash-indexed.hpp"
#include "lib/symbol.hpp"
//#include "lib/util.hpp"

#include <string>


namespace stage {
namespace interact {
  
//  using lib::HashVal;
  using lib::hash::LuidH;
  using lib::Symbol;
//  using util::isnil;
  using std::string;
  
  
  
  /**
   * Abstract foundation of UI state tracking components.
   * @todo write type comment...
   */
  class CmdContext
    : util::NonCopyable
    {
      
    public:
     ~CmdContext();  ///< @todo do we need a VTable / virtual dtor?
      
      /* === access front-end === */
      static CmdContext& of (Symbol cmdID, string ctxID);
      
      struct Resolver;
      
    protected:
    private:
    };
  
  
  /**
   * Helper for dynamic command argument resolution.
   */
  struct CmdContext::Resolver
    {
      
      /** query current interaction state to resolve the element in question
       * @return hash-ID to indicate the element or scope
       */
      operator LuidH ();
      
      
      enum Spec {
        HERE,
        RECENT
      };
      
      
      friend Resolver
      scope (Spec scopeSpec)
      {
        return Resolver{scopeSpec};
      }
      
      
      friend Resolver
      element (Spec elementSpec)
      {
        return Resolver{elementSpec};
      }
      
    private:
      const Spec spec = HERE;
      
      Resolver(Spec s)
        : spec(s)
        { }
    };
  
  
}} // namespace stage::interact
#endif /*STAGE_INTERACT_CMD_CONTEXT_H*/
