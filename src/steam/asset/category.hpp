/*
  CATEGORY.hpp  -  tree like classification of Assets

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


/** @file category.hpp
 ** Definition of Asset categorisation
 */


#ifndef STEAM_ASSET_CATEGORY_H
#define STEAM_ASSET_CATEGORY_H

#include "lib/symbol.hpp"
#include "lib/hash-standard.hpp"

#include <string>
#include <boost/functional/hash.hpp>



namespace steam {
namespace asset {
  
  using lib::Literal;
  
  using std::string;
  using std::ostream;
  
  /**
   * top-level distinction of different Kinds of Assets.
   * For convenience, this classification is slightly denormalised,
   * as AUDIO, and VIDEO are both asset::Media objects, EFFECT and CODEC
   * are asset::Proc objects, while STRUCT and META refer directly to 
   * the corresponding Interfaces asset::Struct and asset::Meta.
   */
  enum Kind
    { AUDIO
    , VIDEO
    , EFFECT
    , CODEC
    , STRUCT                                                     //////////////////////////////////TICKET #1156 : do we need the distinction between STRUCT and META?
    , META
    };
  
  /************************************************************************************//**
   * Tree like classification of Assets.
   * By virtue of the Category, Assets can be organised in nested bins (folders).
   * This includes the distinction of different kinds of Assets, like Audio, Video, Effects...
   * 
   * @remark the path in the tree constitutes a type classification scheme
   * @todo could be far more elaborate. could be a singleton like centralised tree, while
   *       just holding references to Category nodes in the individual Asset. At the moment,
   *       we just use the most simplistic implementation and handle Category objects 
   *       using value semantics.
   */
  class Category
    {
      
      Kind kind_;
      string path_;
      
    public:
      Category (const Kind root, Literal subfolder ="")
        : kind_(root), path_(subfolder) {};
      
      bool operator== (Category const& other) const { return kind_== other.kind_ && path_== other.path_; }
      bool operator!= (Category const& other) const { return kind_!= other.kind_ || path_!= other.path_; }
      
      bool hasKind  (Kind refKind)    const         { return kind_ == refKind; }
      bool isWithin (Category const&) const;
      void setPath  (string const& newpath)         { this->path_ = newpath; }
      
      
      operator string ()  const;
      
      friend size_t hash_value (Category const&);
      
      
      int
      compare (Category const& co)  const
        {
          int res = int(kind_) - int(co.kind_);
          if (0 != res) 
            return res;
          else
            return path_.compare (co.path_);
        }
      
    };
  
  
  inline size_t
  hash_value (Category const& cat)
  {
    size_t hash = 0;
    boost::hash_combine(hash, cat.kind_);
    boost::hash_combine(hash, cat.path_);
    return hash;
  }
  
  
  
}} // namespace steam::asset
#endif
