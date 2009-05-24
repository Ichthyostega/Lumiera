/*
  HASH-INDEXED.hpp  -  generic hash based and typed ID
 
  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>
 
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 
*/


/** @file hash-indexed.hpp
 ** A template for generating hash based ID tags carrying compile-time type info.
 ** While the actual storage is assumed to be based on a POD, the type info is crucial
 ** to circumvent the problems with an "object" base class. Frequently, the need to
 ** manage some objects in a central facility drives us to rely on a common base class,
 ** irrespective of an actual common denominator in the semantics of the objects to
 ** be managed within this collection. Typically this results in this common base class
 ** being almost worthless as an API or interface, causing lots of type casts when using
 ** such a common object management facility. Passing additional context or API information
 ** on a metaprogramming level through the management interface helps avoiding these
 ** shortcomings. 
 ** 
 ** Here we build an ID facility with the following properties:
 ** - based on a configurable storage/implementation of the actual hash or index code.
 ** - tied to a specific hierarchy of objects (template parameter "BA")
 ** - providing an additional template parameter to pass the desired type info
 ** - establishing an type hierarchy relation between ID related to the base class
 **   and the IDs denoting specific subclasses, such that the latter can stand-in
 **   for the generic ID.
 ** - providing a Mixin, which allows any hierarchy to use this facility without 
 **   much code duplication, including an adapter for tr1::unordered_map
 **
 ** @see HashIndexed_test
 ** @see Placement usage example
 **
 */



#ifndef LIB_HASH_INDEXED_H
#define LIB_HASH_INDEXED_H

extern "C" {
#include "lib/luid.h"
}
#include <functional>


namespace lib {
  
  /** Hash implementations usable for the HashIndexed mixin
   *  as well as key within tr1::unordered_map */
  namespace hash {
    
    /** 
     * simple Hash implementation
     * directly incorporating the hash value.
     */
    class Plain
      {
        const size_t hash_;
        
      public:
        Plain (size_t val)
          : hash_(val)
          { }
        
        template<typename TY>
        Plain (TY const& something)
          : hash_(hash_value (something))  // ADL
          { }
        
        operator size_t()  const { return hash_; }
      };
    
    /**
     * Hash implementation based on a lumiera unique object id (LUID)
     * When invoking the default ctor, a new LUID is generated
     */
    class LuidH 
      {
        lumiera_uid luid_;
        
      public:
        LuidH ()
          {
            lumiera_uid_gen (&luid_);
            ENSURE (0 < lumiera_uid_hash(&luid_));
          }
        
        typedef lumiera_uid* LUID;
        
        operator size_t ()                const { return lumiera_uid_hash ((LUID)&luid_); }
        bool operator== (LuidH const& o)  const { return lumiera_uid_eq ((LUID)&luid_, (LUID)&o.luid_); }
        bool operator!= (LuidH const& o)  const { return !operator== (o); }
        
        /** for passing to C APIs */
        LUID get() { return &luid_; }
      };
    
    
    /* === for use within unordered_map === */
    size_t hash_value (Plain const& plainHash)  { return plainHash; }
    size_t hash_value (LuidH const& luid_Hash)  { return luid_Hash; }
    
  } // namespace "hash"
  
  
  
  /************************************************************
   * A Mixin to add a private ID type to the target class,
   * together with storage to hold an instance of this ID,
   * getter and setter, and a templated version of the ID type
   * which can be used to pass specific subclass type info.
   */
  template<class BA, class IMP>
  struct HashIndexed
    {
      
      /**
       * generic hash based ID, corresponding to the base class BA
       */
      struct ID : IMP
        {
          ID ()              : IMP ()            {}
          ID (BA const& ref) : IMP (ref.getID()) {}
          ID (IMP const& ir) : IMP (ir)          {}
        };
      
      /** 
       * Hash based ID, typed to a specific subclass of BA
       */
      template<typename T>
      struct Id : ID
        {
          Id ()             : ID ()    {}
          Id (T const& ref) : ID (ref) {}
        };
      
      
      /** enables use of BA objects as keys within tr1::unordered_map */
      struct UseEmbeddedHash
        : public std::unary_function<BA, size_t>
        {
          size_t operator() (BA const& obj)  const { return obj.getID(); }
        };
      
      /** trivial hash functor using the ID as hash */
      struct UseHashID
        : public std::unary_function<ID, size_t>
        {
          size_t operator() (ID const& id)  const { return id; }
        };
      
      
      ID const&
      getID ()  const
        { 
          return id_; 
        }
      
      void
      assignID (HashIndexed const& ref)
        { 
          this->id_ = ref.getID();
        }
      
    protected:
      HashIndexed ()                : id_()     {}
      HashIndexed (IMP const& iref) : id_(iref) {}
      
    private:
      ID id_;
    };
  
  
  
  
  
} // namespace lib
#endif
