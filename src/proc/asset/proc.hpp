/*
  PROC.hpp  -  key abstraction: media-like assets
 
  Copyright (C)         CinelerraCV
    2007,               Hermann Vosseler <Ichthyostega@web.de>
 
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


/** @file proc.hpp
 ** Data processing Plugins and Codecs can be treated as a specific Kind of Asset.
 ** For the different <i>Kinds</i> of Assets, we use sub-intefaces inheriting
 ** from the general Asset interface. To be able to get asset::Proc instances
 ** directly from the AssetManager, we define a specialization of the Asset ID.
 **
 ** @see asset.hpp for explanation
 ** @see ProcFactory creating concrete asset::Proc instances
 **
 */

#ifndef ASSET_PROC_H
#define ASSET_PROC_H

#include "proc/asset.hpp"
#include "common/factory.hpp"



namespace asset
  {
  
  class Proc;
  class ProcFactory;
  
  typedef shared_ptr<const Proc> PProc;
  
  
  
  template<>
  class ID<Proc> : public ID<Asset>
    {
    public:
      ID (size_t id);
      ID (const Proc&);
    };

    
    
  /**
   * key abstraction: data processing asset
   * @todo just a stub, have to figure out what a asset::Proc is
   */
  class Proc : public Asset
    {
    public:
      static ProcFactory create;
      
      virtual const ID<Proc>& getID()  const    ///< @return ID of kind Proc 
        { 
          return static_cast<const ID<Proc>& > (Asset::getID()); 
        }
      
    protected:
      Proc (const Asset::Ident& idi) : Asset(idi) {}  //////////////TODO
      friend class ProcFactory;
    };
    
    
    // definition of ID<Proc> ctors is possible now,
   //  after providing full definition of class Proc

  inline ID<Proc>::ID(size_t id)        : ID<Asset> (id)           {};
  inline ID<Proc>::ID(const Proc& proc) : ID<Asset> (proc.getID()) {};
  
  
  
  
  /** 
   * Factory specialized for createing Processor Asset objects.
   */ 
  class ProcFactory : public cinelerra::Factory<asset::Proc>
    {
    public:
      typedef shared_ptr<asset::Proc> PType;
       
      PType operator() (Asset::Ident& key);      ////////////TODO define actual operation 

    };

    
    
    
} // namespace asset
#endif
