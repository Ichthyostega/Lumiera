/*
  DB.hpp  -  registry holding known Asset instances.
 
  Copyright (C)         CinelerraCV
    2007,               Christian Thaeter <ct@pipapo.org>
 
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


#ifndef ASSET_DB_H
#define ASSET_DB_H


#include "proc/asset.hpp"

#include <tr1/unordered_map>
#include <boost/utility.hpp>


namespace asset
  {
  using std::tr1::static_pointer_cast;
  using std::tr1::dynamic_pointer_cast;
  
    
  /* ===== hash implementations ===== */
  
  size_t 
  hash_value (const Asset::Ident& idi)
  {
    size_t hash = 0;
    boost::hash_combine(hash, idi.org);
    boost::hash_combine(hash, idi.name);
    boost::hash_combine(hash, idi.category);
    return hash;
  }
  
  size_t
  hash_value (const Asset& asset)
  {
    return asset.getID();
  }
  
  
  /** 
   * trivial hash functor.
   * returns any hash value unmodified.
   * For building a hashtable with keys 
   * already containing valid hash values.
   */
  struct IdentityHash 
    : public std::unary_function<size_t, size_t>
    {
      size_t 
      operator() (size_t val) const   { return val; }
    };
  
  typedef std::tr1::unordered_map<size_t, PAsset, IdentityHash> IdHashtable;
  
  
  
  
  /**
   * Implementation of the registry holding all Asset 
   * instances known to the Asset Manager subsystem. 
   * As of 8/2007 implemented by a hashtable.
   */
  class DB : private boost::noncopyable
    {
      IdHashtable table;
      
      DB () : table() {}
      ~DB ()          {}
      
      friend class cinelerra::singleton::Static<DB>;
      
    public:
      template<class KIND>
      void  put (ID<KIND> hash, shared_ptr<KIND>& ptr) { table[hash] = static_pointer_cast (ptr);  }
      void  put (ID<Asset> hash, PAsset& ptr)          { table[hash] = ptr;   }
      
      template<class KIND>
      shared_ptr<KIND> 
      get (ID<KIND> hash)
        {
          return dynamic_pointer_cast<KIND,Asset> (table[hash]);
        }
      
      /** intended for diagnostics */
      void 
      asList (list<PAsset>& output) const  
        { 
          IdHashtable::const_iterator i = table.begin(); 
          IdHashtable::const_iterator e = table.end(); 
          for ( ; i!=e ; ++i )  
            output.push_back (i->second);
        }
    };
  
    
} // namespace asset
#endif
