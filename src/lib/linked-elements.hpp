/*
  LINKED-ELEMENTS.hpp  -  configurable intrusive single linked list template

   Copyright (C)
     2012,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file linked-elements.hpp
 ** Intrusive single linked list with optional ownership.
 ** This helper template allows to attach a number of tightly integrated
 ** elements with low overhead. Typically, these elements are to be attached
 ** once and never changed. Optionally, elements can be created using a
 ** custom allocation scheme; the holder might also take ownership. These
 ** variations in functionality are controlled by policy templates.
 ** 
 ** The rationale for using this approach is
 ** - variable number of elements
 ** - explicit support for polymorphism
 ** - no need to template the holder on the number of elements
 ** - no heap allocations (contrast this to using std::vector)
 ** - clear and expressive notation at the usage site
 ** - convenient access through Lumiera Forward Iterators
 ** - the need to integrate tightly with a custom allocator
 ** 
 ** @note effectively, LinkedElements is „just a dressed-up pointer“;
 **       under the assumption that the allocator is a _monostate_, and
 **       exploiting _empty-base-optimisation,_ footprint is minimal.
 ** @note this linked list container is _intrusive_ and thus needs the help
 **       of the element type, which must *provide a pointer member* `next`.
 **       Consequently, each such node element can not be member in
 **       several collections at the same time (unless they share all elements)
 ** @note as usual, any iterator relies on the continued existence and
 **       unaltered state of the container. There is no sanity check.
 ** @warning this container is deliberately not threadsafe
 ** @warning be sure to understand the implications of taking ownership:
 **       When adding an existing node element (by pointer) to a LinkedElements
 **       list which takes ownership, the list will attempt to destroy or de-allocate
 **       this node element when going out of scope.
 ** 
 ** @see LinkedElements_test
 ** @see allocator-handle.hpp
 ** @see ScopedCollection
 ** @see itertools.hpp
 */


#ifndef LIB_LINKED_ELEMENTS_H
#define LIB_LINKED_ELEMENTS_H


#include "lib/error.hpp"
#include "lib/nocopy.hpp"
#include "lib/iter-adapter.hpp"
#include "lib/allocator-handle.hpp"
#include "lib/util.hpp"

#include <utility>
#include <memory>


namespace lib {
  
  namespace error = lumiera::error;
  using LERR_(INDEX_BOUNDS);
  using util::unConst;
  
  
  namespace linked_elements { ///< allocation policies for the LinkedElements list container
    
    /**
     * Policy for LinkedElements: taking ownership and
     * possibly creating new Nodes through a custom allocator.
     * @tparam ALO a C++ standard conformant allocator
     * @note is util::MoveOnly to enforce ownership
     *       on behalf of LinkedElements
     */
    template<class ALO>
    struct OwningAllocated
      : lib::allo::StdFactory<ALO>
      , util::MoveOnly
      {
        using lib::allo::StdFactory<ALO>::StdFactory;
        
        using CustomAllocator = ALO;
      };
    
    
    /** default policy: use standard heap allocation for new elements */
    template<class N>
    using OwningHeapAllocated = OwningAllocated<std::allocator<N>>;
    
    
    
    
    /**
     * Policy for LinkedElements: never create or destroy
     * any elements, only allow to add already existing nodes.
     * @note any added node needs to provide a `next` pointer
     *       field, which is used ("intrusively") for managing
     *       the list datastructure. But besides that, the
     *       node element won't be altered or discarded
     *       in any way.
     */
    struct NoOwnership
      {
        typedef void* CustomAllocator;
        
        /** this policy doesn't take ownership
         *  and thus never discards anything
         */
        void
        dispose (void*)
          {
            /* does nothing */
          }
        
        template<class TY, typename...ARGS>
        TY&
        create (ARGS&&...)
          {
            static_assert ("NoOwnership allocation strategy can not allocate elements");
          } //          ... you'll have to do that yourself (that's the whole point of using this strategy)
      };
    
  }//(END)namespace linked_elements (predefined policies)
  
  
  
  
  
  
  
  
  
  
  /****************************************************************************//**
   * Intrusive single linked list, possibly taking ownership of node elements.
   * Additional elements can be pushed (prepended) to the list; element access
   * is per index number (slow) or through an Lumiera Forward Iterator traversing
   * the linked list. There is no support for changing the list contents aside
   * of discarding any element in the list.
   * 
   * The allocation and ownership related behaviour is controlled by a policy class
   * provided as template parameter. When this policy supports creation of new
   * elements, these might be created and prepended in one step.
   * @note with the default policy, `sizeof(LinkedElements) == sizeof(N*)`
   */
  template
    < class N       ///< node class or Base/Interface class for nodes
    , class ALO = linked_elements::OwningHeapAllocated<N>
    >
  class LinkedElements
    : ALO
    {
      N* head_;
      
      
    public:
      
     ~LinkedElements()  noexcept
        {
          clear();
        }
      
      LinkedElements()
        : head_{nullptr}
        { }
      
      LinkedElements (LinkedElements const&) = default;
      LinkedElements (LinkedElements&& rr)
        : head_{rr.head_}
        { // possibly transfer ownership
          rr.head_ = nullptr;
        }
      
      /** @param allo custom allocator or memory manager
       *         to be used by the policy for creating and
       *         discarding of node elements
       */
      explicit
      LinkedElements (typename ALO::CustomAllocator allo)
        : ALO{allo}
        , head_{nullptr}
        { }
      
      /**
       * creating a LinkedElements list in RAII-style:
       * @param elements iterator to provide all the elements
       *        to be pushed into this newly created collection
       * @note any exception while populating the container
       *        causes already created elements to be destroyed
       */
      template<class IT>
      LinkedElements (IT&& elements)
        : head_{nullptr}
        {
        try {
            pushAll (std::forward<IT> (elements));
          }
        catch(...)
          {
            WARN (progress, "Failure while populating LinkedElements list. "
                            "All elements will be discarded");
            clear();
            throw;
        } }
      
      
      
      /** @note EX_FREE */
      void
      clear()  noexcept
        {
          while (head_)
            {
              N* elm = head_;
              head_ = head_->next;
              try {
                  ALO::dispose (elm);
                }
              ERROR_LOG_AND_IGNORE (progress, "Clean-up of element in LinkedElements list")
            }
        }
      
      
      /** convenience shortcut to add all the elements
       *  yielded by the given Lumiera Forward Iterator
       *  @note EX_SANE
       */
      template<class IT>
      void
      pushAll (IT elements)
        {
          for ( ; elements; ++elements )
            push (*elements);
        }
      
      
      
      
      /** accept the given element and prepend it to the list of elements;
       *  depending on the allocation policy, this might imply taking ownership
       * @note EX_STRONG
       */
      template<typename TY>
      TY&
      push (TY& elm)  noexcept
        {
          elm.next = head_;
          head_ = &elm;
          return elm;
        }
      
      
      
      /** prepend object of type TY, forwarding ctor args */
      template<class TY =N, typename...ARGS>
      TY&
      emplace (ARGS&& ...args)
        {
          return push (* ALO::template create<TY> (std::forward<ARGS> (args)...));
        }
      
      
      
      /**
       * Mutate the complete list to change the order of elements.
       * @remark since pushing prepends, elements are initially in reverse order
       * @warning this operation invalidates iterators and has O(n) cost;
       *          ownership and elements themselves are not affected.
       */
      LinkedElements&
      reverse()
        {
          if (not empty())
            {
              N* p = head_->next;
              head_->next = nullptr;
              while (p)
                {
                  N& n = *p;
                  p = p->next;
                  push (n);
            }   }
          return *this;
        }
      
      
      
      
      /* === Element access and iteration === */
      
      N&
      operator[] (size_t index)  const
        {
          N* p = head_;
          while (p and index)
            {
              p = p->next;
              --index;
            }
          
          if (!p or index)
            throw error::Logic ("Attempt to access element beyond the end of LinkedElements list"
                               , LERR_(INDEX_BOUNDS));
          else
            return *p;
        }
      
      
      /** @warning unchecked */
      N&
      top()  const
        {
          REQUIRE (head_, "NIL");
          return *(unConst(this)->head_);
        }
      
      
      /** @warning traverses to count the elements */
      size_t
      size()  const
        {
          uint count=0;
          N* p = head_;
          while (p)
            {
              p = p->next;
              ++count;
            }
          return count;
        }
      
      
      bool
      empty() const
        {
          return !head_;
        }
      
    private:
      /**
       * Iteration is just following the single linked list.
       * We encapsulate this simple pointer into a dedicated marker type
       * to ease the handling and mixing of iterators and const iterators.
       * (Explanation: IterationState depends on our type parameters...)
       */
      struct IterationState
        {
          N* node;
          
          IterationState (N* p =nullptr)
            : node{p}
            { }
          
          /* ==== internal callback API for the iterator ==== */
          
          /** Iteration-logic: switch to next position
           * @warning assuming the given node pointer belongs to this collection.
           *          actually this is never checked; also the given node might
           *          have been deallocated in the meantime.
           */
          void
          iterNext()
            {
              node = node->next;
            }
          
          /** Iteration-logic: detect iteration end. */
          bool
          checkPoint()  const
            {
              return bool(node);
            }
          
          N&
          yield()  const
            {
              REQUIRE (node);
              return * unConst(this)->node;
            }
          
          friend bool
          operator== (IterationState const& il, IterationState const& ir)
          {
            return il.node == ir.node;
          }
        };
      
    public:
      using       iterator = IterStateWrapper<IterationState,       N&>;
      using const_iterator = IterStateWrapper<IterationState, const N&>;
      
      
      iterator       begin()       { return iterator       (head_); }
      const_iterator begin() const { return const_iterator (head_); }
      iterator       end ()        { return iterator();       }
      const_iterator end ()  const { return const_iterator(); }
      
    };
  
  
  /** transiently reinterpret an element pointer as const LinkedElements,
   *  allowing to count, iterate or subscript a chain of elements */
  template<class N>
  auto&
  asLinkedElements (N* const& anchor)
  {
    using Linked = LinkedElements<const N,linked_elements::NoOwnership>;
    return reinterpret_cast<Linked const&>(anchor);
  }
  
  
  
} // namespace lib
#endif /*LIB_LINKED_ELEMENTS_H*/
