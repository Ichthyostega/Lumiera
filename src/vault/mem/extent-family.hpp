/*
  EXTENT-FAMILY.hpp  -  maintain a sequence of memory extents used cyclically

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


/** @file extent-family.hpp
 ** Memory management scheme for cyclically used memory extents.
 ** In this context, _Extent_ denotes a larger block of memory to hold a
 ** cluster of smaller records, which are closely interrelated and will be managed
 ** and de-allocated together. The typical usage involves a constant demand for further
 ** memory, with is satisfied by putting further unused extents into use; older extents
 ** will fall out of use eventually, and can then be placed back into a buffer of free
 ** extents. In accordance to overall demand, this reserve buffer can be scaled up
 ** and down to avoid holding larger amounts of unused memory, while the availability
 ** of a baseline amount of memory can be enforced.
 ** 
 ** @see ExtentFamily_test
 ** @see gear::BlockFlow usage example
 */


#ifndef SRC_VAULT_MEM_EXTENT_FAMILY_H_
#define SRC_VAULT_MEM_EXTENT_FAMILY_H_


#include "vault/common.hpp"
#include "lib/uninitialised-storage.hpp"
#include "lib/iter-adapter.hpp"
#include "lib/nocopy.hpp"
#include "lib/util.hpp"

#include <algorithm>
#include <memory>
#include <vector>
#include <array>


namespace vault{
namespace mem {
  
  namespace {
    const size_t ALLOC_SAFETY_LIMIT = 8_GiB;
  }
  
  using util::unConst;
  
  template<typename T, size_t siz>
  class ExtentDiagnostic;
  
  
  
  /**
   * Memory manager to provide a sequence of Extents for cyclic usage.
   * Storage extents, once allocated, will be re-used without invoking any
   * ctors or dtors on the objects nominally located in »slots« within the Extent.
   * @tparam T payload type living in the Extent's »slots«
   * @tparam siz number of »slots« per Extent
   * @see ExtentFamily_test
   */
  template<typename T, size_t siz>
  class ExtentFamily
    : util::NonCopyable
    {
      /** number of excess new extents to add
       *  whenever new storage is required */
      static const size_t EXCESS_ALLOC{5};
      
      
    public:
      /** logical structure of a memory Extent */
      struct Extent
        : std::array<T,siz>
        {
          using Payload = T;
          using SIZ = std::integral_constant<size_t,siz>;
        };
      
      
    private:
      using _UniqueStoragePtr = std::unique_ptr<lib::UninitialisedStorage<T,siz>>;
      
      /** Entry in the Extents management datastructure */
      struct Storage
        : _UniqueStoragePtr
        {
          /**
           * @note default ctor immediately allocates the full storage,
           *       but without any initialisation of memory content
           */
          Storage()
            : _UniqueStoragePtr{new lib::UninitialisedStorage<T,siz>}
            { }
          
          /** access projected Extent storage type
           * @warning payload is uninitialised and dtors won't be invoked
           */
          Extent&
          access()
            {
              auto* rawStorage = this->get();
              ENSURE (rawStorage != nullptr);
              return static_cast<Extent&> (rawStorage->array());
            }
        };
      using Extents = std::vector<Storage>;
      
      
      /**
       * Iteration »State Core« based on Extents index position.
       * @remark builds upon the specific wrap-around logic for
       *         cyclic reuse of extent storage and allocation.
       */
      struct IdxLink
        {
          ExtentFamily* exFam{nullptr};
          size_t        index{0};
          
          /* === state protocol API for IterStateWrapper === */
          bool
          checkPoint()  const
            {
              return exFam and index != exFam->after_;
            }
          
          Extent&
          yield()  const
            {
              return exFam->access (index);
            }
          
          void
          iterNext()
            {
              index = exFam->incWrap (index);
            }
          
          bool
          operator== (IdxLink const& oi)  const
            {
              return exFam == oi.exFam
                 and index == oi.index;
            }
          
          
          /* === pass-through extended functionality === */
          
          size_t getIndex() { return index;    }
          
          void
          expandAlloc (size_t cnt =1)
            {
              size_t prevStart = exFam->start_;
              exFam->openNew(cnt);
              if (index >= prevStart)
                index += (exFam->start_-prevStart);
                // was in a segment that might be moved up
              ENSURE (exFam->isValidPos (index));
            }
        };
      
      
      /* ==== Management Data ==== */
      
      Extents extents_;
      size_t start_,after_;
      
    public:
      explicit
      ExtentFamily(size_t initialCnt =1)
        : extents_{initialCnt}
        , start_{0}        //  Extents allocated yet marked unused
        , after_{0}
        { }
      
      void
      reserve (size_t expectedMaxExtents)
        {                // pertaining management data only
          extents_.reserve (expectedMaxExtents);
        }
      
      /** claim next \a cnt extents, possibly allocate.
       * @remark the index pos previously marked as #after_
       *         will always become the first new storage slot.
       * @warning in case of #isWrapped, #first_ will be modified
       *         and thus _any existing indices may be invalidated._
       * @note   always allocating slightly in excess
       */
      void
      openNew (size_t cnt =1)
        {
          if (not canAccomodate (cnt))
            {//insufficient reserve => allocate
              size_t oldSiz = slotCnt();
              size_t addSiz = cnt - freeSlotCnt()
                              + EXCESS_ALLOC;
              // add a strike of new extents at the end
              ___sanityCheckAllocSize (addSiz);
              extents_.resize (oldSiz + addSiz);
              if (isWrapped())
                {// need the new elements in the middle, before the existing start_
                  auto p = extents_.begin();
                  auto first = p + start_;
                  auto mid   = p + oldSiz;
                  auto last  = p + oldSiz + addSiz;
                  // relocate [fist..mid) after [mid..last)
                  std::rotate (first, mid, last);
                  start_ += addSiz;
                }
            }
          // now sufficient reserve extents are available
          ENSURE (canAccomodate (cnt));
          after_ = incWrap (after_, cnt);
        }
      
      /** discard oldest \a cnt extents */
      void
      dropOld (size_t cnt)
        {
          REQUIRE (cnt <= activeSlotCnt());
          start_ = incWrap (start_, cnt);
        }                        ////////////////////////////////////////////////////////////////////////////TICKET #1316 : should reduce excess allocation (with appropriate damping to avoid oscillations)
      
      
      /** allow transparent iteration of Extents,
       *  with the ability to expand storage */
      using iterator = lib::IterableDecorator<Extent, IdxLink>;
      
      /** iterate over all the currently active Extents */
      iterator begin() { return iterator{IdxLink{this, start_}}; }
      iterator end()   { return iterator{IdxLink{this, after_}}; }
      
      friend iterator begin (ExtentFamily& exFam) { return exFam.begin(); }
      friend iterator end   (ExtentFamily& exFam) { return exFam.end();   }
      
      
      bool empty()    const { return start_ == after_; }
      
      /** positioned to the last / latest storage extent opened
       * @warning undefined behaviour when empty
       */
      iterator
      last()
        {
          REQUIRE (not empty());             // trick to safely decrement by one
          size_t penultimate = incWrap (after_, slotCnt()-1);
          return iterator{IdxLink{this, penultimate}};
        }
      
      
    private: /* ====== storage management implementation ====== */
      bool
      isWrapped()  const
        {
          return after_ < start_;
        }     // note: both are equal only when empty
      
      size_t
      slotCnt()  const
        {
          return extents_.size();
        }
      
      /** @return number of allocated slots actually used */
      size_t
      activeSlotCnt()  const
        {
          REQUIRE (start_ <  slotCnt());
          REQUIRE (after_ <= slotCnt());
          
          return not isWrapped()? after_ - start_
                                : (after_ - 0)
                                 +(slotCnt() - start_);
        }
      
      size_t
      freeSlotCnt()  const
        {           // always keep one in reserve...
          REQUIRE (activeSlotCnt() < slotCnt());
          
          return slotCnt() - activeSlotCnt();
        }
      
      bool
      canAccomodate (size_t addCnt)  const
        {
          return addCnt < freeSlotCnt();
        }     // keep one in reserve to allow for
             //  unambiguous iteration end in wrapped state
      
      /** increment index, but wrap at array end.
       * @remark using the array cyclically
       */
      size_t
      incWrap (size_t idx, size_t inc =1)
        {
          return (idx+inc) % slotCnt();
        }
      
      bool
      isValidPos (size_t idx)  const
        {
          REQUIRE (idx < slotCnt());
          REQUIRE (activeSlotCnt() > 0);
          
          return isWrapped()? (start_ <= idx and idx < slotCnt())
                               or idx < after_
                            : (start_ <= idx and idx < after_);
        }
      
      Extent&
      access (size_t idx)  const
        {
          REQUIRE (isValidPos (idx));
          return unConst(this)->extents_[idx].access();
        }     // deliberately const-ness does not cover payload
      
      void
      ___sanityCheckAllocSize (size_t addCnt)
        {
          size_t resultSiz = slotCnt()+addCnt;
          size_t requiredSpace = resultSiz * sizeof(Extent);
          using namespace lumiera::error;
          if (requiredSpace > ALLOC_SAFETY_LIMIT)
            throw Fatal{"Raw allocation exceeds safety limit: "
                         +util::showSize(requiredSpace)    +" > "
                         +util::showSize(ALLOC_SAFETY_LIMIT)
                       , LUMIERA_ERROR_CAPACITY};
        }
      
      
      /// „backdoor“ to watch internals from tests
      friend class ExtentDiagnostic<T,siz>;
    };
  
  
  
  
  
  
  
  /* ===== Test / Diagnostic ===== */
  
  template<typename T, size_t siz>
  struct ExtentDiagnostic
    {
      using ExFam = ExtentFamily<T,siz>;
      
      ExFam& exFam_;
      
      size_t first()  { return exFam_.start_; }
      size_t last()   { return exFam_.after_; }
      size_t size()   { return exFam_.slotCnt(); }
      size_t active() { return exFam_.activeSlotCnt(); }
    };
  
  template<typename T, size_t siz>
  inline ExtentDiagnostic<T,siz>
  watch (ExtentFamily<T,siz>& extentFamily)
  {
    return ExtentDiagnostic<T,siz>{extentFamily};
  }
  
}} // namespace vault::mem
#endif /*SRC_VAULT_MEM_EXTENT_FAMILY_H_*/
