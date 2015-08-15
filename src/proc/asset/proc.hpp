/*
  PROC.hpp  -  key abstraction: data-processing assets

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file proc.hpp
 ** Data processing Plugins and Codecs can be treated as a specific Kind of Asset.
 ** For the different <i>Kinds</i> of Assets, we use sub-interfaces inheriting
 ** from the general Asset interface. To be able to get asset::Proc instances
 ** directly from the AssetManager, we define a specialisation of the Asset ID.
 ** 
 ** @ingroup asset
 ** @see asset.hpp for explanation
 ** @see ProcFactory creating concrete asset::Proc instances
 **
 */

#ifndef ASSET_PROC_H
#define ASSET_PROC_H

#include "proc/asset.hpp"
#include "proc/streamtype.hpp"

#include <boost/noncopyable.hpp>



namespace proc {
namespace asset {
  
  
  class Proc;
  class ProcFactory;
  
  typedef lib::P<const Proc> PProc;
  
  
  
  template<>
  class ID<Proc> : public ID<Asset>
    {
    public:
      ID (HashVal id);
      ID (const Proc&);
    };
  
  
  
  /**
   * key abstraction: data processing asset
   * @todo just a stub, have to figure out what a asset::Proc is
   * @ingroup asset
   * @ingroup session
   */
  class Proc : public Asset
    {
    public:
      static ProcFactory create;
      
      virtual const ID<Proc>& getID()  const    ///< @return ID of kind Proc 
        { 
          return static_cast<const ID<Proc>& > (Asset::getID()); 
        }
      
      typedef StreamType::ImplFacade::DataBuffer Buff;
      typedef Buff* PBuff;
      typedef void (ProcFunc) (PBuff);
      
      
      /** resolve any plugin and configuration info
       *  to yield the actual media data processing function.
       *  @return a function ready to be invoked; either the 
       *          "real thing" or a suitable placeholder.
       *  @throw lumiera::error::Fatal if unable to provide
       *         any usable function or placeholder. This case
       *         can be considered exceptional and justifies a
       *         subsystem failure.
       */
      virtual ProcFunc*
      resolveProcessor()  const =0;
      
      
      
    protected:
      Proc (const Asset::Ident& idi) : Asset(idi) {}  //////////////TODO
      friend class ProcFactory;
    };
  
  
    // definition of ID<Proc> ctors is possible now,
   //  after providing full definition of class Proc
  
  inline ID<Proc>::ID(HashVal id)       : ID<Asset> (id)           {};
  inline ID<Proc>::ID(const Proc& proc) : ID<Asset> (proc.getID()) {};
  
  
  
  
  /** 
   * Factory specialised for creating Processor Asset objects.
   */ 
  class ProcFactory
    : boost::noncopyable
    {
    public:
      typedef lib::P<asset::Proc> PType;
       
      PType operator() (Asset::Ident& key);      ////////////TODO define actual operation 
      
    };
  
  
  
  
}} // namespace proc::asset
#endif
