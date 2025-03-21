/*
  DEL-STASH.hpp  -  collect and execute deleter functions

   Copyright (C)
     2010,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file del-stash.hpp
 ** Collecting and finally triggering deleter functions.
 ** This building block for custom memory management allows memorising how to kill an object.
 ** Frequently, custom allocation schemes have to deal with type erased elements, where the
 ** full typed context is only available during construction. When implementing these objects
 ** without vtable, we need a reliable way of recalling the correct destructor to invoke.
 ** Typically, such entry objects are to be de-allocated in bulk during shutdown, with
 ** the possibility to deallocate some objects beforehand explicitly.
 ** 
 ** The implementation is based on using a vector in a stack-like fashion, thus the
 ** deallocation of individual objects might degenerate in performance.
 ** 
 ** @see DelStash_test
 ** @see AdviceSystem usage example
 **
 */



#ifndef LIB_DEL_STASH_H
#define LIB_DEL_STASH_H


#include "lib/error.hpp"
#include "lib/symbol.hpp"
#include "lib/nocopy.hpp"

#include <algorithm>
#include <vector>


namespace lib {
  
  
  /**
   * Manage a collection of deleter functions.
   * This component can memorise addresses and deleter functions
   * and trigger deletion of single objects, or delete all objects
   * on demand or automatically on shutdown.
   * @warning clients must not add a given object more than once
   */
  class DelStash
    : util::NonCopyable
    {
      
      typedef void KillFun(void*);
      
      /**
       * @internal entry to store target pointer
       * and the actual deleter function to use
       */
      class Killer
        {
          void* target_;
          KillFun* killIt_;
          
        public:
          Killer(KillFun* f, void* t)
            : target_{t}
            , killIt_{f}
            {
              REQUIRE(f);
            }
          
          void
          trigger()
            {
              if (target_)
                killIt_(target_);
              target_ = NULL; // remember kill
            }
          
          bool operator== (const void* target)  const { return target_ == target; }
          bool operator!= (const void* target)  const { return target_ != target; }
          
          bool
          isActive()  const
            {
              return bool(target_);
            }
        };
      
      
      typedef std::vector<Killer> Killers;
      
      Killers killers_;
      
      
    public:
      DelStash (size_t elms_to_reserve =0)
        : killers_()
        {
          if (elms_to_reserve)
            {
              killers_.reserve (elms_to_reserve);
            }
        }
      
     ~DelStash ()
        try { killAll(); }
        
        catch(...)
          {
            Symbol errID = lumiera_error();
            WARN (memory, "Problems on de-allocation: %s", errID.c());
          }
      
      
      size_t
      size ()  const
        {
          size_t activeEntries = 0;
          size_t i = killers_.size();
          while (i)
            if (killers_[--i].isActive())
              ++activeEntries;
          return activeEntries;
        }
      
      
      
      template<typename TY>
      void
      manage (TY* obj)//,  __disable_if_voidPtr<TY> =0)
        {
          if (!obj) return;
          REQUIRE (!isRegistered (obj));
          killers_.push_back (Killer (how_to_kill<TY>, obj));
        }
      
      template<typename TY>
      void
      manage (TY& obj)//,  __disable_if_voidPtr<TY> =0)
        {
          REQUIRE (!isRegistered (&obj));
          killers_.push_back (Killer (how_to_kill<TY>, &obj));
        }
      
      void
      manage (void* obj, KillFun* customDeleter)
        {
          if (!obj) return;
          REQUIRE (!isRegistered (obj));
          killers_.push_back (Killer (customDeleter, obj));
        }
      
      
      template<typename TY>
      void
      kill (TY* obj)
        {
          triggerKill (obj);
        } // note: entry remains in the killer vector,
         //  but is disabled and can't be found anymore
      
      template<typename TY>
      void
      kill (TY& obj)
        {
          triggerKill (&obj);
        }
      
      void
      killAll ()
        {
          size_t i = killers_.size();
          while (i)
            killers_[--i].trigger();
        }
      
      
    private:
      /** trampoline function to invoke destructor
       *  of the specific target type */
      template<typename X>
      static void
      how_to_kill (void* subject)
        {
          X* victim = static_cast<X*> (subject);
          ENSURE (victim);
          delete victim;
        };
      
      bool
      isRegistered (const void* objAddress)
        {
          REQUIRE (objAddress);
          return killers_.end() != findEntry (objAddress);
        }
      
      Killers::iterator
      findEntry (const void* obj)
        {
          REQUIRE (obj);
          return std::find (killers_.begin(),killers_.end(), obj);
        }
      
      void
      triggerKill (void* objAddress)
        {
          Killers::iterator pos = findEntry (objAddress);
          if (killers_.end() != pos)
            pos->trigger();
          ENSURE (!isRegistered (objAddress), "duplicate deleter registration");
        }
    };
  
  
  
  
} // namespace lib
#endif
