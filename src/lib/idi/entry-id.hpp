/*
  ENTRY-ID.hpp  -  plain symbolic and hash ID used for accounting

  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file entry-id.hpp
 ** Bare symbolic and hash ID used for accounting of asset like entries.
 ** This ID can be used to register instances with an accounting table, without all the
 ** overhead of creating individual assets for each entry. The datafields in the symbolic part
 ** of the ID are similar to the asset identity tuple; the idea is to promote individual entries
 ** to full fledged assets on demand. Alongside with the symbolic identity, which can be reduced
 ** to just a Symbol and (compile time) type information, we store the derived hash value as LUID.
 ** 
 ** @note as of 3/2010 this is an experimental setup and exists somewhat in parallel
 **       to the assets. We're still in the process of finding out what's really required
 **       to keep track of all the various kinds of objects.                 ///////////////////TICKET #739
 ** 
 ** @see asset::Asset::Ident
 ** @see entry-id-test.cpp
 ** 
 */


#ifndef LIB_IDI_ENTRY_ID_H
#define LIB_IDI_ENTRY_ID_H


#include "lib/error.hpp"
#include "lib/hash-indexed.hpp"
#include "lib/idi/genfunc.hpp"
#include "lib/util.hpp"

#include <boost/functional/hash.hpp>
#include <boost/operators.hpp>
#include <iostream>
#include <string>


namespace lib {

  /**
   * Identification Schemes.
   * Collection of commonly used mechanisms to build identification records,
   * unique identifiers, registration numbers and hashes. These are used as glue
   * and thin abstraction to link various subsystems or to allow interoperation
   * of registration facilities
   */
namespace idi {
  
  namespace error = lumiera::error;
  
  using std::string;
  using std::ostream;
  
  using lib::idi::generateSymbolicID;
  using lib::idi::getTypeHash;
  using lib::idi::typeSymbol;
  using lib::hash::LuidH;
  using lib::HashVal;
  
  namespace {
    
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
     *        a new random value. How to make EntryID and asset::Ident interchangeable,  /////////TICKET #739
     *        which would require both to yield the same hash values....
     *  @warning there is a weakness in boost::hash for strings of running numbers, causing
     *        collisions already for a small set with less than 100000 entries.
     *        To ameliorate the problem, we hash the symbol twice                        /////////TICKET #865
     *  @warning this code isn't portable and breaks if sizeof(size_t) < sizeof(void*)
     */
    inline LuidH
    buildHash (string const& sym, HashVal seed =0)
    {
      boost::hash_combine(seed, sym);
      boost::hash_combine(seed, sym);                                     ////////////////////////TICKET #865
      lumiera_uid tmpLUID;
      lumiera_uid_set_ptr (&tmpLUID, reinterpret_cast<void*> (seed));
      return reinterpret_cast<LuidH&> (tmpLUID);
    }
  }
  
  
  
  template<class TY>
  struct EntryID;
  
  
  /**
   * type erased baseclass
   * for building a combined hash and symbolic ID.
   */
  class BareEntryID
    : public boost::equality_comparable<BareEntryID>
    {
      
      string symbol_;
      LuidH hash_;
      
    protected:
      /** 
       * Not to be created stand-alone.
       * derived classes feed down the specific type information
       * encoded into a hash seed. Thus even the same symbolicID
       * generates differing hash-IDs for different type parameters
       */
      BareEntryID (string const& symbolID, HashVal seed =0)
        : symbol_(util::sanitise(symbolID))
        , hash_(buildHash (symbol_, seed))
        { }
      
    public:
      /* default copyable and assignable */
      
      
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
      
      
      /** using BareEntryID derived objects as keys within tr1::unordered_map */
      struct UseEmbeddedHash
        : public std::unary_function<BareEntryID, size_t>
        {
          size_t operator() (BareEntryID const& obj)  const { return obj.getHash(); }
        };
      
      
      template<typename TAR>
      EntryID<TAR> recast()  const;
      
    };
  
  
  /** 
   * typed symbolic and hash ID for asset-like position accounting.
   * Allows for creating an entry with symbolic id and distinct type,
   * combined with an derived hash value, without the overhead in storage
   * and instance management imposed by using a full-fledged Asset.
   * 
   * Similar to an Asset, an identification tuple is available (generated on the fly),
   * as is an unique LUID and total ordering. The type information is attached as
   * template parameter, but included into the hash calculation. All instantiations of the
   * EntryID template share a common baseclass, usable for type erased common registration.
   * @todo currently storing the symbolic-ID as string. It should be a lib::Symbol,
   *       but this isn't possible unless we use a symbol table. //////TICKET #158
   * @warning there is a weakness in boost::hash applied to strings. EntryID by default
   *       generates symbolic IDs from a type prefix plus a running number, which causes
   *       collisions already with less than 100000 entries.
   * @todo As a temporary workaround, we hash the symbolic ID twice, but we should look
   *       into using a better hash function               ////////////TICKET #865
   * 
   * @see mobject::session::Fork
   */
  template<class TY>
  struct EntryID
    : BareEntryID
    , boost::totally_ordered< EntryID<TY> >
    {
      
      /** case-1: auto generated symbolic ID */
      EntryID()
        : BareEntryID (generateSymbolicID<TY>(), getTypeHash<TY>())
        { }
      
      /** case-2: explicitly specify a symbolic ID to use.
       *  The type information TY will be included automatically
       *  into the generated hash-ID. This hash is reproducible.
       */
      explicit
      EntryID (string const& symbolID)
        : BareEntryID (symbolID, getTypeHash<TY>())
        { }
      
      
      /** @return true if the upcast would yield exactly the same
       *  tuple (symbol,type) as was used on original definition
       *  of an ID, based on the given BareEntryID. Implemented
       *  by re-calculating the hash.
       */
      static bool
      canRecast (BareEntryID const& bID)
        {
          return bID.getHash() == buildHash (bID.getSym(), getTypeHash<TY>());
        }
      
      static EntryID
      recast (BareEntryID const& bID)
        {
          if (!canRecast(bID))
            throw error::Logic ("unable to recast EntryID: desired type "
                                "doesn't match original definition"
                               , error::LUMIERA_ERROR_WRONG_TYPE);
          return EntryID (bID.getSym());
        }
      
      
      operator string ()  const
        {
          return "ID<"+typeSymbol<TY>()+">-"+EntryID::getSym();
        }
      
      friend ostream& operator<<   (ostream& os, EntryID const& id) { return os << string(id); }
      friend bool operator<  (EntryID const& i1, EntryID const& i2) { return i1.getSym()  < i2.getSym(); }
    };
    
  
  inline bool
  operator== (BareEntryID const& i1, BareEntryID const& i2)
  {
    return i1.getHash() == i2.getHash();
  }
  
  
  
  
  /** try to upcast this BareEntryID to a fully typed EntryID.
   *  Effectively, this is the attempt to reverse a type erasure;
   *  thus the caller needs to know the type information (as provided
   *  by the template parameter), because this information can't be
   *  recovered from the stored data.
   *  @throws error::Logic if the given type parameter isn't exactly
   *          the same as was used on creation of the original EntryID,
   *          prior to type erasing it into a BareEntryID. Implemented
   *          by re-calculating the hash from typeinfo + symbolicID;
   *          Exception if it doesn't match the stored hash.
   */
  template<typename TAR>
  EntryID<TAR> BareEntryID::recast()  const
  {
    return EntryID<TAR>::recast(*this);
  }
  
  
  
}} // namespace lib::idi
#endif /*LIB_IDI_ENTRY_ID_H*/
