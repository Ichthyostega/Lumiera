/*
  VISITOR-DISPATCHER.hpp  -  visitor implementation details

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file visitor-dispatcher.hpp
 ** Helper for a trampoline table based implementation of the visitor pattern.
 ** @internal implementation part, clients should include visitor.hpp
 */



#ifndef LUMIERA_VISITOR_DISPATCHER_H
#define LUMIERA_VISITOR_DISPATCHER_H

#include "lib/error.hpp"
#include "lib/util.hpp"
#include "lib/sync-classlock.hpp"
#include "lib/depend.hpp"
#include "lib/util.hpp"

#include <vector>


namespace lib {
namespace visitor {
  
  using lib::ClassLock;
  using lib::Depend;
  
  
  template<class TOOL> class Tag;
  
  
  template<class TOOL, class TOOLImpl>
  struct TagTypeRegistry 
    {
      static Tag<TOOL> tag;
    };
    
  /**
   * Type tag for concrete visiting tool classes.
   * Used to access the previously registered dispatcher 
   * trampoline function when handling a visitor invocation.
   */
  template<class TOOL>
  class Tag
  {
    size_t tagID; ///< tag value
    static size_t lastRegisteredID;
    
    static void
    generateID (size_t& id)
      {
        ClassLock<Tag> guard;
        if (!id)
          id = ++lastRegisteredID;
      }
    
  public:
    Tag() : tagID(0) { }
    operator size_t()  const { return tagID; }
    
    
    template<class TOOLImpl>
    static Tag&
    get (TOOLImpl* const =0)
      {
        Tag& t = TagTypeRegistry<TOOL,TOOLImpl>::tag;
        if (!t) generateID (t.tagID);
        return t;
      }
  
  };
  
  
  
  /** storage for the Tag registry for each concrete tool */
  template<class TOOL, class TOOLImpl>
  Tag<TOOL> TagTypeRegistry<TOOL,TOOLImpl>::tag; 
  
  template<class TOOL>
  size_t Tag<TOOL>::lastRegisteredID (0);
  
  
  
  
  
  
  
  
  /**
   * For each possible call entry point via some subclass of the visitable hierarchy,
   * we maintain a dispatcher table to keep track of all concrete tool implementations
   * able to receive and process calls on objects of this subclass.
   * @param TAR the concrete target (subclass) type within the visitable hierarchy
   * @param TOOL the overall tool family (base class of all concrete tools)
   */
  template<class TAR, class TOOL>
  class Dispatcher
    {
      typedef typename TOOL::ReturnType ReturnType;
      
      /** generator for Trampoline functions,
       *  used to dispatch calls down to the 
       *  right "treat"-Function on the correct
       *  concrete tool implementation class
       */
      template<class TOOLImpl>
      static ReturnType 
      callTrampoline (TAR& obj, TOOL& tool)
        {
          // cast down to real implementation type
          REQUIRE (INSTANCEOF (TOOLImpl, &tool));  
          TOOLImpl& toolObj = static_cast<TOOLImpl&> (tool);
          
          // trigger (compile time) overload resolution
          // based on concrete type, then dispatch the call.
          // Note this may cause obj to be upcasted.
          return toolObj.treat (obj);
        }
      
      typedef ReturnType (*Trampoline) (TAR&, TOOL& );
      
      
      /** VTable for storing the Trampoline pointers */
      std::vector<Trampoline> table_;
      
      
      void
      accomodate (size_t index)
        {
          ClassLock<Dispatcher> guard;    // note: this lock is also used for the singleton!
          if (index > table_.size())
            table_.resize (index);      // performance bottleneck?? TODO: measure the real impact!
        }
      
      inline bool
      is_known (size_t id)
        { 
          return id<=table_.size() && table_[id-1]; 
        }
      
      inline void 
      storePtr (size_t id, Trampoline func)
        {
          REQUIRE (func);
          REQUIRE (0 < id);
          if (id>table_.size())
            accomodate (id);
          table_[id-1] = func;
        }
      
      inline Trampoline 
      storedTrampoline (size_t id)
        {
          if (id<=table_.size() && table_[id-1])
            return table_[id-1];
          else
            return &errorHandler;
        }
      
      static ReturnType
      errorHandler (TAR& target, TOOL& tool)
        {
          return tool.onUnknown (target);
        }
      
      
    public:
      static Depend<Dispatcher<TAR,TOOL>> instance;
      
      inline ReturnType 
      forwardCall (TAR& target, TOOL& tool)
        {
          // get concrete type via tool's VTable 
          Tag<TOOL> index = tool.getTag();
          return (*storedTrampoline(index)) (target, tool);
        }
      
      template<class TOOLImpl>
      inline void 
      enrol(TOOLImpl* typeref)
        {
          Tag<TOOL>& index = Tag<TOOL>::get (typeref);
          if (is_known (index))
            return;
          else
            {
              Trampoline func = &callTrampoline<TOOLImpl>;
              storePtr (index, func);
            }
            
        }
    };
  
  /** storage for the dispatcher table(s) */
  template<class TAR, class TOOL>
  Depend<Dispatcher<TAR,TOOL>> Dispatcher<TAR,TOOL>::instance;
  
  
  
  
}} // namespace lib::visitor
#endif
