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
 ** @see ////TODO_test usage example
 ** @see gear::BlockFlow usage example
 ** 
 ** @todo WIP-WIP-WIP 7/2023 »Playback Vertical Slice«
 ** 
 */


#ifndef SRC_VAULT_MEM_EXTENT_FAMILY_H_
#define SRC_VAULT_MEM_EXTENT_FAMILY_H_


#include "vault/common.hpp"
//#include "lib/symbol.hpp"
#include "lib/iter-adapter.hpp"
#include "lib/nocopy.hpp"
//#include "lib/util.hpp"

//#include <string>
#include <memory>
#include <vector>
#include <array>


namespace vault{
namespace mem {
  
//  using util::isnil;
//  using std::string;
  template<typename T, size_t siz>
  class ExtentDiagnostic;

  
  /**
   * Memory manager to provide a sequence of Extents for cyclic usage.
   * @todo WIP-WIP 7/2023
   * @see NA_test
   */
  template<typename T, size_t siz>
  class ExtentFamily
    : util::NonCopyable
    {
    public:
      struct Extent
        : std::array<T,siz>
        {
          using Payload = T;
          using SIZ = std::integral_constant<size_t,siz>;
        };
      
    private:
      struct Storage
        : std::unique_ptr<char[]>
        {
          /**
           * @note default ctor immediately allocates the full storage,
           *       but without initialisation since payload is `char`
           */
          Storage()
            : unique_ptr{new char[sizeof(Extent)]}
            { }
          
          /** access projected Extent storage type
           * @warning payload is uninitialised and dtors won't be invoked
           */
          Extent&
          access()
            {
              ENSURE (get() != nullptr);
              return reinterpret_cast<Extent&> (*get());
            }
        };
      using Extents = std::vector<Storage>;
      using RawIter = typename Extents::iterator;
      
      
      Extents extents_;
      size_t start_,after_;
      
    public:
      explicit
      ExtentFamily(size_t initialCnt =0)
        : extents_{initialCnt}
        , start_{0}        //  Extents allocated yet marked unused
        , after_{0}
        { }
      
      void
      reserve (size_t expectedMaxExtents)
        {
          extents_.reserve (expectedMaxExtents);
        }
      
      void
      openNew (size_t cnt)
        {
          UNIMPLEMENTED ("claim next cnt extents, possibly allocate");
        }
      
      void
      dropOld (size_t cnt)
        {
          UNIMPLEMENTED ("discard oldest cnt extents");
        }
      
      
      /** allow transparent iteration of Extents, expanding storage on demand */
      using iterator = lib::IterAdapter<RawIter, ExtentFamily*>;     ////////////////////OOO consider to use a Extent* instead of the RawIter??
      
      
      iterator
      active()
        {
          UNIMPLEMENTED ("visit all active extents, possibly claim next ones");
        }
      
      
      /* == Iteration control API (used by IterAdapter via ADL) == */
      
      friend bool
      checkPoint (ExtentFamily* exFam, RawIter& pos)
      {
        UNIMPLEMENTED ("ExtentFamily iteration control: check and adapt position");
      }
      
      friend void
      iterNext (ExtentFamily* exFam, RawIter& pos)
      {
        UNIMPLEMENTED ("ExtentFamily iteration control: access next Extent, possibly expand allocation");
      }
      
      friend class ExtentDiagnostic<T,siz>;
    };
  
  
  
  template<typename T, size_t siz>
  struct ExtentDiagnostic
    {
      using ExFam = ExtentFamily<T,siz>;
      
      ExFam& exFam_;
      
      size_t first()  { return exFam_.start_; }
      size_t last()   { return exFam_.after_; }
      size_t size()   { return exFam_.extents_.size(); }
      size_t active() { UNIMPLEMENTED ("count active"); }
    };
  
  template<typename T, size_t siz>
  inline ExtentDiagnostic<T,siz>
  watch (ExtentFamily<T,siz>& extentFamily)
  {
    return ExtentDiagnostic<T,siz>{extentFamily};
  }
  
}} // namespace vault::mem
#endif /*SRC_VAULT_MEM_EXTENT_FAMILY_H_*/
