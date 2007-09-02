/*
  ASSET.hpp  -  Superinterface: bookeeping view of "things" present in the session
 
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


#ifndef PROC_INTERFACE_ASSET_H
#define PROC_INTERFACE_ASSET_H

#include <string>
#include <vector>
#include <set>
#include "common/error.hpp"

using std::string;
using std::vector;
using std::set;


namespace asset { class Category; }


namespace proc_interface
  {
  
  typedef void* PAsset; //////TODO
  
  /**
   * Superinterface describing especially the bookeeping properties of Assets
   */
  class Asset
    {
    public:
      /** Asset primary key. */
      const long id;

      /** element ID, comprehensible but sanitized.
       *  The tuple (category, name, org) is unique.
       */
      const string name;

      /**primary tree like classification of the asset  */
      const asset::Category* category;

      /** origin or authorship id. 
       *  Can be a project abbreviation, a package id or just the authors nickname or UID.
       *  This allows for the compnent name to be more generic (e.g. "blur"). 
       *  Default for all assets provided by the core cinelerra-3 codebase is "cin3".
       */
      const string org;

      /** version number of the thing or concept represented by this asset.
       *  Of each unique tuple (name, category, org) there will be only one version 
       *  in the whole system. Version 0 is reserved for internal purposes. 
       *  Versions are considered to be ordered, and any higher version is 
       *  supposed to be fully backwards compatible to all previous versions.
       */
      const unsigned int version;


    protected:
      /** additional classification, selections or departments this asset belongs to.
       *  Groups are optional, non-exclusive and may be overlapping.
       */
      set<string> groups;

      /** user visible Name-ID. To be localized. */
      const string shortDesc;

      /** user visible qualification of the thing, unit or concept represented by this asset.
       *  perferably "in one line". To be localized.  */
      const string longDesc;


    public:
      /** List of entities this asset depends on or requires to be functional. 
       *  May be empty. The head of this list can be considered the primary prerequisite
       */
      vector<PAsset> getParents () ;
      
      /** All the other assets requiring this asset to be functional. 
       *  For example, all the clips depending on a given media file. 
       *  May be empty. The dependency relation is transitive.
       */
      vector<PAsset> getDependant () ;
      
      /** weather this asset is swithced on and consequently 
       *  included in the fixture and participates in rendering
       */
      bool isActive () ;
      
      /** change the enabled status of this asset.
       *  Note the corresponding #isActive predicate may 
       *  depend on the enablement status of parent assets as well
       */
      void enable ()  throw(cinelerra::error::State);
    };

} // namespace proc_interface



namespace asset
  {
  using proc_interface::Asset;
}

#endif
