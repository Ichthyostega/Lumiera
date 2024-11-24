/*
  UNINITIALISED-STORAGE.hpp  -  array-like container with raw storage

   Copyright (C)
     2023,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file uninitialised-storage.hpp
 ** A raw memory block with proper alignment and array access.
 ** This is a building block for custom containers and memory management schemes.
 ** Regular containers in C++ always ensure invocation of any object's constructors
 ** and destructors -- which is a boon and prevents a lot of consistency problems.
 ** When constructing custom management schemes however, this automatic initialisation
 ** can be problematic; some objects require constructor arguments, preventing mass
 ** initialisation; and initialising a large memory block has considerable cost,
 ** which may be wasted it the intended usage is to plant objects into that space
 ** later, on demand. The std::vector::reserve(size) function can often be used
 ** to circumvent those problems — yet unfortunately std::vector has the hard
 ** requirement on the content objects to be at least _movable_ to support
 ** automatic storage growth.
 ** 
 ** A traditional workaround is to exploit a programming trick relying on a
 ** _forced cast:_ Storage is declared as `char[]` without initialiser; since
 ** `char` is a primitive type, C++ will not default-initialise the storage then
 ** for sake of plain-old-C compatibility. A special accessor function will then
 ** force-cast into the desired target type. However well established, this
 ** practice is riddled with various problems
 ** - it requires very precise setup and any mistake leads to memory corruption
 ** - according to the C++ standard, such an access involves _undefined behaviour_
 ** - compilers are getting better and increasingly aggressive with performing global
 **   optimisation and may miss the secondary hidden access under some circumstances.
 ** - when objects with `const` data fields are placed into such a buffer, the compiler
 **   may perform constant folding based on the current/initial object values and thus
 **   fail to propagate changes due to other object instances being placed into storage.
 ** 
 ** Attempts were made to codify this kind of usage properly into the standard; these
 ** turned out to be problematic however and were [deprecated again]. Starting with
 ** C++17, fortunately there is now a way to express this kind of raw unprotected
 ** access through standard conformant ways and marked clearly to prevent overly
 ** zealous optimisation. Since the typical use is for allocating a series of
 ** storage slots with a well defined target type, this implementation
 ** relies on std::array as »front-end« for access.
 ** - target type `T` and size are given as template parameters
 ** - the storage is defined as `std::byte` to express its very purpose
 ** - the secondary access is marked by `std::launder` to prevent optimisation
 ** - an implicit conversion path to the corresponding array type is provided
 ** - subscript operators enable direct access to the raw storage
 ** - helper functions allow for placement new and delete.
 ** [deprecated again]: https://stackoverflow.com/a/71828512
 ** @see extent-family.hpp
 ** @see vault::gear::TestChainLoad::Rule where this setup matters
 */


#ifndef LIB_UNINITIALISED_STORAGE_H
#define LIB_UNINITIALISED_STORAGE_H


#include <cstddef>
#include <utility>
#include <array>
#include <new>


namespace lib {
  
  /**
   * Block of raw uninitialised storage with array like access.
   * @tparam T   the nominal type assumed to sit in each »slot«
   * @tparam cnt number of »slots« in the array
   */
  template<typename T, size_t cnt>
  class UninitialisedStorage
    {
      using _Arr = std::array<T,cnt>;
      alignas(T) std::byte buffer_[sizeof(_Arr)];
      
    public:
      _Arr&
      array()
        {
          return * std::launder (reinterpret_cast<_Arr* > (&buffer_));
        }
      
      _Arr const&
      array()  const
        {
          return * std::launder (reinterpret_cast<_Arr const*> (&buffer_));
        }
      
      
      operator _Arr&()                        { return array(); }
      operator _Arr const&()            const { return array(); }
      
      T &      operator[] (size_t idx)        { return array()[idx]; }
      T const& operator[] (size_t idx)  const { return array()[idx]; }
      
      
      template<typename...Args>
      T&
      createAt (size_t idx, Args&& ...args)
        {
          return *new(&operator[](idx)) T{std::forward<Args>(args)...};
        }
      
      void
      destroyAt (size_t idx)
        {
          operator[](idx).~T();
        }
      
      static constexpr size_t size() { return cnt; }
    };
  
  
  
  
  /**
   * Managed uninitialised Heap-allocated storage with array like access.
   * @tparam T the nominal type assumed to sit in each »slot«
   */
  template<typename T>
  class UninitialisedDynBlock
    {
      using _Arr = T[];
      
      void*  buff_{nullptr};
      size_t size_{0};
      
    public:
      T*
      allocate(size_t cnt)
        {
          if (buff_) discard();
          size_ = cnt;
          buff_ = cnt? std::aligned_alloc (std::alignment_of<T>(), cnt * sizeof(T))
                     : nullptr;
          return front();
        }
      
      void
      discard()
        {
          std::free (buff_);
          buff_ = nullptr;
          size_ = 0;
        }
      
      
      UninitialisedDynBlock()  =default;
     ~UninitialisedDynBlock()
        {
          if (buff_)
            discard();
        }
      explicit
      UninitialisedDynBlock (size_t cnt)
        {
          if (cnt)
            allocate(cnt);
        }
      
      UninitialisedDynBlock (UninitialisedDynBlock && rr)
        {
          if (this != &rr)
            swap (*this, rr);
        }
      
      UninitialisedDynBlock (UninitialisedDynBlock const&)            =delete;
      UninitialisedDynBlock& operator= (UninitialisedDynBlock &&)     =delete;
      UninitialisedDynBlock& operator= (UninitialisedDynBlock const&) =delete;
      
      friend void
      swap (UninitialisedDynBlock& u1, UninitialisedDynBlock& u2)
      {
        using std::swap;
        swap (u1.size_, u2.size_);
        swap (u1.buff_, u2.buff_);
      }
      
      explicit
      operator bool()  const
        {
          return bool(buff_);
        }
      
      size_t
      size()  const
        {
          return size_;
        }
      
      
      _Arr&
      array()
        {
          return * std::launder (reinterpret_cast<_Arr* > (buff_));
        }
      
      _Arr const&
      array()  const
        {
          return * std::launder (reinterpret_cast<_Arr const*> (buff_));
        }
      
      
      T *      front()       { return &array()[0]; }
      T const* front() const { return &array()[0]; }
      T *      after()       { return &array()[size_];}
      T const* after() const { return &array()[size_];}
      T *      back ()       { return after() - 1; }
      T const* back () const { return after() - 1; }
      
      T &      operator[] (size_t idx)        { return array()[idx]; }
      T const& operator[] (size_t idx)  const { return array()[idx]; }
      
      
      template<typename...Args>
      T&
      createAt (size_t idx, Args&& ...args)
        {
          return *new(&operator[](idx)) T{std::forward<Args>(args)...};
        }
      
      void
      destroyAt (size_t idx)
        {
          operator[](idx).~T();
        }
    };
  
  
} // namespace lib
#endif /*LIB_UNINITIALISED_STORAGE_H*/
