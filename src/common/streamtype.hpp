/*
  STREAMTYPE.hpp  -  classification of media stream types 
 
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


#ifndef LUMIERA_STREAMTYPE_HPP
#define LUMIERA_STREAMTYPE_HPP


#include "common/query.hpp"

#include <boost/noncopyable.hpp>


namespace lumiera
  {


  /**
   * 
   */
  struct StreamType : boost::noncopyable
    {
      enum MediaKind
        {
          VIDEO,
          IMMAGE,
          AUDIO,
          MIDI
        };
      
      enum Usage
        {
          RAW,
          SOURCE,
          TARGET,
          INTERMEDIARY
        };
      
      struct Prototype;
      
      class ImplFacade;
      
      
      MediaKind kind;
      Prototype const& prototype;
      ImplFacade * implType;
      Usage usageTag;
      
    };
  
  
  
  /**
   * 
   */
  struct StreamType::Prototype
    {
      Symbol id;
      
      bool subsumes (Prototype const& other)  const;
      bool canConvert (Prototype const& other)  const;
    };
  
  
  
  /**
   * 
   */
  class StreamType::ImplFacade
    {
    public:
      Symbol libraryID;
      
    };
    
  

   
} // namespace lumiera
#endif
