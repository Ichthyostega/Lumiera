/*
  COMMAND-REGISTRY.hpp  -  steam-Command object registration and storage management

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


/** @file command-registry.hpp
 ** Managing command definitions and the storage of individual command objects.
 ** The CommandRegistry is an singleton object, accessible only at the implementation level
 ** of control::Command (note: CommandImpl isn't tied to the registry). For the other parts
 ** of the command system, it provides all "get me this command object"-services. Actually,
 ** these can be decomposed into two distinct parts:
 ** - allocation of CommandImpl frames and argument holders, which is delegated
 **   to the TypedAllocationManager
 ** - maintaining an index to find pre-built command definitions (prototypes)
 ** 
 ** # Services during command lifecycle
 ** 
 ** Each command starts out as command definition, accessed by client code through CommandDef.
 ** While collecting the necessary parts of such a definition, there is just an empty (pending)
 ** Command (smart-ptr frontend), which is not yet usable, being held within the CommandDef. 
 ** When the definition is complete, a CommandImpl frame is allocated, configured and used to
 ** activate the Command (smart-ptr frontend), at which point it also gets accessible 
 ** through the CommandRegistry.
 ** 
 ** Later on, client code is assumed to re-access the command by ID. It may bind arguments, which are
 ** stored in the already allocated StorageHolder. (-->Ticket #269). As the Command frontend is a
 ** smart-ptr, commands may be copied, stored away and passed on. When finally the ref-count of
 ** a given definition goes to zero, de-allocation happens automatically. This can't happen for
 ** a registered command definition though, as a Command instance is stored within the index
 ** table, keeping the linked data alive. Thus, any registered commands will remain in memory
 ** until de-registered explicitly, or until application shutdown.
 ** 
 ** @see Command
 ** @see SteamDispatcher
 **
 */



#ifndef CONTROL_COMMAND_REGISTRY_H
#define CONTROL_COMMAND_REGISTRY_H

#include "lib/error.hpp"
#include "lib/depend.hpp"
#include "lib/sync.hpp"
#include "include/logging.h"
#include "lib/nocopy.hpp"
#include "lib/util.hpp"

#include "steam/control/command.hpp"
#include "steam/control/command-signature.hpp"
#include "steam/control/command-storage-holder.hpp"
#include "lib/typed-allocation-manager.hpp"

#include <boost/functional/hash.hpp>
#include <unordered_map>
#include <memory>
#include <string>
#include <map>



namespace steam {
namespace control {
  
  using boost::hash;
  using std::shared_ptr;
  using std::unordered_map;
  using lib::TypedAllocationManager;
  using util::getValue_or_default;
  using util::contains;
  using std::string;
  using std::map;
  
  
  /**
   * Helper for building a std::map with
   * Command* as keys. Defines the order
   * by the address of the Command's 
   * implementation object.
   */
  struct order_by_impl
    {
      bool
      operator() (const Command *pC1, const Command *pC2)  const
        {
          return (!pC1 && pC2)
              || ( pC1 && pC2  && (*pC1 < *pC2));
        }
    };
  
  
  /**
   * Registry managing command implementation objects (Singleton).
   * Relies on TypedAllocationManager for pooled custom allocation (TODO: not implemented as of 9/09)
   * Registered command (definitions) are accessible by command ID;
   * as this mapping is bidirectional, it is also possible to find
   * out the ID for a given command.
   */
  class CommandRegistry
    : public lib::Sync<>
    , util::NonCopyable
    {
      // using a hashtable to implement the index
      typedef unordered_map<Symbol, Command, hash<Symbol>> CmdIndex;
      typedef map< const Command*, Symbol, order_by_impl> ReverseIndex;
      
      TypedAllocationManager allocator_;
      CmdIndex index_;
      ReverseIndex ridx_;
      
      
    public:
      static lib::Depend<CommandRegistry> instance;
      
      
     ~CommandRegistry()
        {
          if (0 < index_size())
            TRACE (command, "Shutting down Command system...");
                                                                //////////////////////TICKET #295 : possibly remotely trigger Command mass suicide here....
          ridx_.clear();
          index_.clear();
        }
      
      
      
      /** register a command (Frontend) under the given ID.
       * @throw error::Logic when a registration already exists,
       *        either under this ID or for the same concrete implementation
       *        record but with a different ID.
       */ 
      void
      track (Symbol cmdID, Command const& commandHandle)
        {
          Lock sync(this);
          
          REQUIRE (commandHandle);
          if (contains (index_,cmdID) || contains(ridx_, &commandHandle))
            commandHandle.duplicate_detected(cmdID);
          
          Command& indexSlot = index_[cmdID];
          indexSlot = commandHandle;
          ridx_[&indexSlot] = cmdID;
          
          ENSURE (contains(ridx_, &indexSlot));
          ENSURE (contains(index_, cmdID));
        }
      
      
      /** remove the given command registration. 
       *  @return `true` if actually removed an entry
       *  @note existing command instances remain valid;
       *        storage will be freed at zero use-count */
      bool
      remove (Symbol cmdID)
        {
          Lock sync(this);
          
          bool actually_remove = contains (index_,cmdID);
          if (actually_remove)
            {
              ridx_.erase(& index_[cmdID]);
              index_.erase(cmdID);
            }
          ENSURE (!contains (index_,cmdID));
          return actually_remove;
        }
      
      
      /** query the command index by ID
       *  @return the registered command,
       *          or an "invalid" token
       *  @remark this function deliberately returns by-value.
       *         Returning a reference into the global CommandRegistry
       *         would be dangerous under concurrent access, since the
       *         lock is only acquired within this function's body.
       */
      Command
      queryIndex (Symbol cmdID)
        {
          Lock sync(this);
          return getValue_or_default (index_, cmdID, Command() );
        }                                           //if not found
      
      
      /** search the command index for a definition
       *  @param cmdInstance using the definition to look up
       *  @return the ID used to register this definition 
       *          or `NULL` in case of an "anonymous" command */
      Symbol
      findDefinition (Command const& cmdInstance)  const
        {
          Lock sync(this);
          return getValue_or_default (ridx_, &cmdInstance, Symbol::BOTTOM );
        }                                   //used as Key
      
      
      size_t
      index_size()  const
        {
          return index_.size();
        }
      
      
      size_t
      instance_count()  const
        {
          return allocator_.numSlots<CommandImpl>();
        }
      
      
      /** set up a new command implementation frame
       *  @return shared-ptr owning a newly created CommandImpl, allocated
       *          through the registry and wired internally to invoke
       *          TypedAllocationManager#destroyElement for cleanup.
       */
      template< typename SIG_OPER    ///< signature of the command operation
              , typename SIG_CAPT    ///< signature for capturing undo state
              , typename SIG_UNDO    ///< signature to undo the command
              >
      shared_ptr<CommandImpl>
      newCommandImpl (function<SIG_OPER>& operFunctor
                     ,function<SIG_CAPT>& captFunctor
                     ,function<SIG_UNDO>& undoFunctor)
        {
          
          // derive the storage type necessary
          // to hold the command arguments and UNDO memento
          typedef typename UndoSignature<SIG_CAPT>::Memento Mem;
          typedef StorageHolder<SIG_OPER,Mem> Arguments;
          
          shared_ptr<Arguments> pArg (allocator_.create<Arguments>());
          
          return allocator_.create<CommandImpl> (pArg, operFunctor,captFunctor,undoFunctor);
        }
      
      
      
      
      /** create an allocation for holding a clone of the given CommandImpl data.
       *  This is a tricky operation, as the CommandImpl after construction erases the
       *  specific type information pertaining the StorageHolder. But this specific
       *  type information is vital for determining the exact allocation size for
       *  the clone StorageHolder. The only solution is to delegate the cloning
       *  of the arguments down into the StorageHolder, passing a reference
       *  to the memory manager for allocating the clone. Actually, we perform
       *  this operation through the help of a visitor, which re-gains the
       *  complete type context and prepares the necessary clone objects;
       *  in a final step, we allocate a new CommandImpl frame and 
       *  initialise it with the prepared clone objects.
       *  @see command.cpp (implementation)
       */
      shared_ptr<CommandImpl>
      createCloneImpl (CommandImpl const& refObject);
      
      
    };
  
  
}} // namespace steam::control
#endif
