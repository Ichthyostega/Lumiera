/*
  ENTRY-ID.hpp  -  plain symbolic and hash ID used for accounting
 
  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>
 
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


/** @file entry-id.hpp
 ** Bare symbolic and hash ID used for accounting of asset like entries.
 ** This ID can be used to register instances with an accounting table, without all the
 ** overhead of creating individual assets for each entry. The datafields in the symbolic part
 ** of the ID are similar to the asset identity tuple; the idea is to promote individual entries
 ** to full fledged assets on demand. Alongside with the symbolic identity, which can be reduced
 ** to just a Symbol and a type identifier, we store the derived hash value as LUID. 
 ** 
 ** @note as of 3/2010 this is an experimental setup and exists somewhat in parallel
 **       to the assets. We're still in the process of finding out what's really required
 **       to keep track of all the various kinds of objects.
 **
 ** @see asset::Asset::Ident
 ** @see entry-id-test.cpp
 ** 
 */


#ifndef ASSET_ENTRY_ID_H
#define ASSET_ENTRY_ID_H


#include "proc/asset.hpp"
#include "proc/asset/struct-scheme.hpp"
#include "lib/hash-indexed.hpp"
#include "lib/util.hpp"

#include <string>


namespace asset {

  using std::string;
  
  namespace idi {
    
    using lib::hash::LuidH;
    
    typedef size_t HashVal;
    
    /** build up a hash value, packaged as LUID.
     *  @param sym symbolic ID-string to be hashed
     *  @param seed (optional) hash value to combine with the sym.
     *  @note This is a half baked preliminary solution. The issue here
     *        is that LUID has a fixed size of 128bit, whereas the hash values
     *        of the std library (and boost) have the smaller and platform dependent
     *        type of \c size_t. This hack here assumes that size_t corresponds to void*,
     *        which is correct for i386 and AMD64. LUID provides a hook for embedding a
     *        void* (setting the trailing bits to zero). Finally we reinterpret the
     *        char[] of the LUID as a LuidH class, which is ugly, but granted to work.
     *  @todo several unsolved design problems. How to deal with std hash values in
     *        conjunction with LUID. How to create a LuidH instance, if not generating
     *        a new random value
     */
    inline LuidH
    buildHash (string const& sym, HashVal seed =0)
    {
      boost::hash_combine(seed, sym);
      lumiera_uid tmpLUID;
      lumiera_uid_set_ptr (&tmpLUID, reinterpret_cast<void*> (&seed));
      return reinterpret_cast<LuidH> (tmpLUID);
    }
  }
  
  
  /**
   * type erased baseclass
   * for building a combined hash and symbolic ID.
   */
  class BareEntryID
    {
      typedef lib::hash::LuidH LuidH;
      
      string symbol_;
      LuidH hash_;
      
    public:
      explicit
      BareEntryID (string const& symbolID, HashVal seed =0)        /////////////TODO couldn't this be protected?
        : symbol_(util::sanitise(symbolID))
        , hash_(buildHash (symbol_, seed))
        { }
      
      
      bool
      isValid()  const
        {
          return bool(hash_); 
        }
      
      string const&
      getSym()  const
        {
          return symbol_;
        }
      
      LuidH const&
      getHash()  const
        {
          return hash_;   
        }
    };
  
  /** 
   * thin ID with blah
   * 
   * @see mobject::session::Track
   */
  template<class TY>
  class EntryID
    : public BareEntryID
    {
    public:
      EntryID()
        : BareEntryID (idi::generateSymbolID<TY>(), getTypeHash())
        { }
      
      explicit
      EntryID (string const& symbolID)
        : BareEntryID (symbolID, getTypeHash<TY>())
        { }
      
      
      /** generate an Asset identification tuple
       *  based on this EntryID's symbolic ID and type information.
       *  The remaining fields are filled in with hardwired defaults.
       */
      Asset::Ident
      getIdent()  const
        {
          Category cat (STRUCT, idi::StructTraits<TY>::catFolder);
          return Asset::Ident (name, cat);           
        }
      
      static idi::HashVal
      getTypeHash()
        {
          Category cat (STRUCT, idi::StructTraits<TY>::catFolder);
          return hash_value (cat);
        }
    };
    
    
    

} // namespace asset
#endif
