/*
  HASH-INDEXED.hpp  -  generic hash based and typed ID

   Copyright (C)
     2009,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

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
 **   much code duplication, including an adapter for std::unordered_map
 ** - equality comparison
 **
 ** @see HashIndexed_test
 ** @see Placement usage example
 **
 */



#ifndef LIB_HASH_INDEXED_H
#define LIB_HASH_INDEXED_H

#include "lib/hash-value.h"
#include "lib/hash-standard.hpp"
#include "lib/error.hpp"

extern "C" {
#include "lib/luid.h"
}

#include <functional>


namespace lib {
  
  /** Hash implementations usable for the HashIndexed mixin
   *  as well as key within std::unordered_map */
  namespace hash {
    
    /** 
     * simple Hash implementation
     * directly incorporating the hash value.
     * @note currently (3/2010) not used
     */
    class Plain
      {
        const HashVal hash_;
        
      public:
        Plain (HashVal val)
          : hash_(val)
          { }
        
        template<typename TY>
        Plain (TY const& something)
          : hash_(hash_value (something))  // ADL
          { }
        
        operator HashVal()  const { return hash_; }
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
        
        operator HashVal()                const { return lumiera_uid_hash (get()); }
        bool operator== (LuidH const& o)  const { return lumiera_uid_eq (get(), o.get()); }
        bool operator!= (LuidH const& o)  const { return not operator== (o); }
        
        /** for passing to C APIs */
        LUID get()                        const { return const_cast<LUID> (&luid_);}
      };
    
    
    /* === for use within unordered_map === */
    inline HashVal hash_value (Plain const& plainHash)  { return plainHash; }
    inline HashVal hash_value (LuidH const& luid_Hash)  { return luid_Hash; }
    
  } // namespace "hash"
  
  
  
  /********************************************************//**
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
          ID (BA const& ref) : IMP (ref.getID()) {}   // note: automatic conversion (e.g. from PlacementMO&)
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
      
      
      /** enables use of BA objects as keys within std::unordered_map */
      struct UseEmbeddedHash
        : public std::unary_function<BA, HashVal>
        {
          HashVal operator() (BA const& obj)  const { return obj.getID(); }
        };
      
      /** trivial hash functor using the ID as hash */
      struct UseHashID
        : public std::unary_function<ID, HashVal>
        {
          HashVal operator() (ID const& id)  const { return id; }
        };
      
      
      ID const&
      getID ()  const
        { 
          return id_; 
        }
      
      /** redefining of the specific type info of the Id is allowed,
       *  as all share the same implementation */
      template<typename T>
      Id<T> const&
      recastID ()  const
        {
          return reinterpret_cast<Id<T> const&> (getID());
        }
      
      void
      assignID (HashIndexed const& ref)
        { 
          this->id_ = ref.getID();
        }
      
      /** equality comparison delegated to the ID implementation */
      friend bool operator== (HashIndexed const& hal, HashIndexed const& har) { return hal.id_==har.id_; }
      friend bool operator!= (HashIndexed const& hal, HashIndexed const& har) { return hal.id_!=har.id_; }
      
      
    protected:
      HashIndexed ()                : id_()     {}
      HashIndexed (IMP const& iref) : id_(iref) {}
      
    private:
      ID id_;
    };
  
  
  
  
  
} // namespace lib
#endif
