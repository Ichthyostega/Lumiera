/*
  DB.hpp  -  registry holding known Asset instances.
 
  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>
 
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
#include "common/error.hpp"

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
      
      DB () : table() { }
      ~DB ()          {clear();}
      
      friend class lumiera::singleton::StaticCreate<DB>;
      
      
    public:
      template<class KIND>
      void  put (ID<KIND> hash, shared_ptr<KIND>& ptr) { table[hash] = static_pointer_cast (ptr);  }
      void  put (ID<Asset> hash, PAsset& ptr)          { table[hash] = ptr; }
      bool  del (ID<Asset> hash)                       { return table.erase (hash); }
      
      template<class KIND>
      shared_ptr<KIND> 
      get (ID<KIND> hash)  const
        {
          return dynamic_pointer_cast<KIND,Asset> (find (hash));
        }
      
      /** removes all registered assets and does something similar 
       *  to Asset::unlink() on each to break cyclic dependencies
       *  (we can't use the real unlink()-function, because this 
       *  will propagate, including calls to the AssetManager.
       *  As the destructor of DB needs to call clear(), this
       *  could result in segfaults. This doesn't seem to be
       *  a problem, though, because we register and process
       *  \e all assets and the net effect is just breaking
       *  any cyclic dependencies) 
       */ 
      void
      clear () throw()
        {
          try
            {
              IdHashtable::iterator i = table.begin(); 
              IdHashtable::iterator e = table.end(); 
              for ( ; i!=e ; ++i )
                i->second->dependants.clear();
              
              table.clear();
            }
          catch (lumiera::Error& EX)
            {
              WARN (oper, "Problems while clearing Asset registry: %s", EX.what());
            }
          catch (...)
            {
              ERROR (oper, "Serious trouble while clearing Asset registry.");
        }   }
      
      
      /** intended for diagnostics */
      void 
      asList (list<PcAsset>& output)  const  
        { 
          IdHashtable::const_iterator i = table.begin(); 
          IdHashtable::const_iterator e = table.end(); 
          for ( ; i!=e ; ++i )  
            output.push_back (i->second);
        }
      
    private:
      const PAsset &
      find (size_t hash)  const
        {
          static const PAsset NULLP;
          IdHashtable::const_iterator i = table.find (hash);
          if (i == table.end())
            return NULLP;  // empty ptr signaling "not found"
          else
            return i->second;
        }
    };
  
    
} // namespace asset
#endif
