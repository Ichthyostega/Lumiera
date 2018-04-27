/*
  SESSION-INTERFACE-MODULES.hpp  -  composing the public session API from several interface modules

  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file session-interface-modules.hpp
 ** Self-contained sub-elements on the Session API.
 ** Part of the Session interface is exposed as self-contained 
 ** interface modules -- both for notational convenience at the
 ** usage site, and for keeping session implementation code manageable.
 ** Clients access these modules as nested parts of the public Session interface
 ** through references exposing just the respective interfaces, while the actual
 ** implementation is located in-place (within the SesssionImpl object), packaged
 ** into a non-public baseclass. 
 ** - the DefsManager acts as frontend to the system of default configurations
 **   and config rules, allowing to retrieve the \em default version of various
 **   kinds of objects
 ** - the top-level Timeline (structural assets) act as facade and entry point
 **   to the high-level-model (session contents). There is an table of timelines,
 **   managed automatically and kept in sync with the session::Binding elements
 **   located directly below model root.
 ** - likewise there is an table of all Sequence (structural assets), which
 **   correspond to the fork roots (roots of "track" trees), attached below model root.
 ** - the ElementQuery interface allows to retrieve specific object 
 **   instances by applying a filter predicate.
 ** 
 ** \par maintaining the link between session, timelines and sequences
 ** 
 ** Timeline and Sequence are implemented as asset::Struct, causing them to be
 ** maintained by the AssetManager, which in turn is attached to the session::Root
 ** (WIP 3/2010: yet to be implemented). Creation and destruction of timelines and
 ** sequences is closely connected to some structural changes within the model
 ** - Timeline is related to session::Binding, where the timelines are leading
 **   and the binding elements are dependent on both a timeline and a sequence
 ** - Sequence is related to a Placement<session::Fork> -- but only if attached
 **   immediately below model root; here the forks are leading and the sequences
 **   are completely dependent.
 ** In any case, ctor and dtor of Timeline and Sequence have to care for proper
 ** registration into the SessionInterfaceModules for timelines and sequences
 ** respectively. This is accomplished by using kind-of a backdoor, a SessionServices (???)
 ** (proc internal API) definition, allowing direct communication on implementation
 ** level, without the need to expose this access point on the public session API.
 ** The lib::ElementTracker configured in ( ??? ) receives these  
 ** calls to maintain a list of asset smart-ptrs
 ** 
 ** @see SessionImpl
 ** @see session-services.hpp
 **
 */


#ifndef PROC_MOBJECT_SESSION_INTERFACE_MODULES_H
#define PROC_MOBJECT_SESSION_INTERFACE_MODULES_H

#include "lib/element-tracker.hpp"
#include "proc/asset/timeline.hpp"
#include "proc/asset/sequence.hpp"
#include "proc/mobject/session/element-query.hpp"
#include "common/query/defs-manager.hpp"




namespace proc {
namespace mobject {
namespace session {
  
  
  typedef lib::ElementTracker<asset::Timeline> TimelineTracker;
  typedef lib::ElementTracker<asset::Sequence> SequenceTracker;
  
  
  /**
   * Collection of implementation components,
   * providing self-contained sub-elements
   * exposed on the public Session API.
   */
  struct SessionInterfaceModules
    : util::NonCopyable
    {
      lumiera::query::DefsManager defaultsManager_;
      mobject::session::ElementQuery elementQueryAPI_;
      TimelineTracker timelineRegistry_;
      SequenceTracker sequenceRegistry_;
      
      SessionInterfaceModules();
     ~SessionInterfaceModules();
    };
  
  
  
  
  /** init element-tracking mechanism for
   *  timelines and sequences on session creation
   */
  inline 
  SessionInterfaceModules::SessionInterfaceModules()
  {
    asset::Timeline::setRegistryInstance (timelineRegistry_);
    asset::Sequence::setRegistryInstance (sequenceRegistry_);
  }
  
  inline
  SessionInterfaceModules::~SessionInterfaceModules()
  {
    if (asset::Sequence::is_attached_to (sequenceRegistry_))
      asset::Sequence::deactivateRegistryLink();
    
    if (asset::Timeline::is_attached_to (timelineRegistry_))  // as session dtor is invoked automatically (smart-ptr),
      asset::Timeline::deactivateRegistryLink();             //  another new session might already have grabbed
  }                                                         //   the Timeline / Sequence registration service.
  
  
  
}}} // namespace proc::mobject::session
#endif
