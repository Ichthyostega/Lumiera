/*
  CATEGORY.hpp  -  tree like classification of Assets
 
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


#ifndef ASSET_CATEGORY_H
#define ASSET_CATEGORY_H

#include <string>
#include <iostream>
#include <boost/functional/hash.hpp>



namespace asset
  {
  using std::string;
  using std::ostream;

      /** top-level distinction of different Kinds of Assets.
       *  For convienience, this classification is slightly denormalized,
       *  as AUDIO, and VIDEO are both asset::Media objects, EFFECT and CODEC
       *  are asset::Proc objects, while STRUCT and META refer directly to 
       *  the corresponding Interfaces asset::Struct and asset::Meta.
       */
      enum Kind
      {
        AUDIO,
        VIDEO,
        EFFECT,
        CODEC,
        STRUCT,
        META
      };
  
  /**
   * Tree like classification of Assets.
   * By virtue of the Category, Assets can be organized in nested bins (folders).
   * This includes the distinction of different kinds of Assets, like Audio, Video, Effects...
   * 
   * @todo could be far more elaborate. could be a singleton like centralized tree, while
   *       just holding references to Catetory nodes in the individual Asset. At the moment,
   *       we use just the most simplistic implementation and handle Category objects 
   *       using value semantics.
   */
  class Category
    {
    public:
      
    private:
      Kind kind_;
      string path_;
      
    public:
      Category (const Kind root, string subfolder ="") 
        : kind_(root), path_(subfolder) {};
      
      bool operator== (const Category& other) const { return kind_== other.kind_ && path_== other.path_; }
      bool operator!= (const Category& other) const { return kind_!= other.kind_ || path_!= other.path_; }
        
      bool hasKind (Kind refKind)     const         { return kind_ == refKind; }
      bool isWithin (const Category&) const;
      
      
      operator string ()  const;

        
      friend size_t hash_value (const Category& cat)
        {
          size_t hash = 0;
          boost::hash_combine(hash, cat.kind_);
          boost::hash_combine(hash, cat.path_);
          return hash;
        }
      
      int compare (const Category& co)  const
        {
          int res = int(kind_) - int(co.kind_);
          if (0 != res) 
            return res;
          else
            return path_.compare (co.path_);
        }

    };
    
   inline ostream& operator<< (ostream& os, const Category& cago) { return os << string(cago); }

   
    
} // namespace asset
#endif
