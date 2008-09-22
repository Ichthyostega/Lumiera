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

/** @file streamtype.hpp
 ** Framework for classification of media streams.
 ** Besides the actual implementation type of a media stream, the Proc-Layer
 ** needs a more general way for accessing, comparing and manipulating media streams
 ** based on type information.
 ** 
 ** @see control::STypeManager
 ** 
 */


#ifndef LUMIERA_STREAMTYPE_HPP
#define LUMIERA_STREAMTYPE_HPP


#include "common/query.hpp"

#include <boost/noncopyable.hpp>


namespace lumiera {


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
      class ImplConstraint;
      
      
      MediaKind kind;
      Prototype const& prototype;
      ImplFacade * implType;
      Usage intentionTag;
      
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
   * placeholder definition for the contents of a data buffer
   */
  struct DataBuffer { };
  
  
  /**
   * A (more or less) concrete implementation type, wired up
   * as a facade providing the basic set of operations.
   */
  class StreamType::ImplFacade
    {
    public:
      Symbol libraryID;
      
      virtual bool operator== (ImplFacade const& other)  const =0;
      virtual bool operator== (StreamType const& other)  const =0;
      
      virtual bool canConvert (ImplFacade const& other)  const =0;
      virtual bool canConvert (StreamType const& other)  const =0;
      
      virtual DataBuffer* createFrame ()  const =0;
      
      virtual ~ImplFacade() {};
      
    protected:
      ImplFacade (Symbol libID) ;
    };
  
  
  /**
   * 
   */
  class StreamType::ImplConstraint
    : public StreamType::ImplFacade
    {
    public:
      Symbol libraryID;
      
      virtual bool canConvert (ImplFacade const& other)  const =0;
      virtual bool canConvert (StreamType const& other)  const =0;
      
      virtual bool subsumes (ImplFacade const& other)  const   =0;
      
      /** modify the other impl type such as to comply with this constraint */
      virtual void makeCompliant (ImplFacade & other)  const   =0;
      
      /** create a default impl type in accordance to this constraint
       *  and use it to create a new framebuffer */
      virtual DataBuffer* createFrame ()  const =0;

      /** similarily create a impl type which complies to this constraint
       *  as well as to the additional constraints (e.g. frame size).
       *  Create a new framebuffer of the resutling type */
      virtual DataBuffer* createFrame (ImplConstraint const& furtherConstraints)  const =0;
      
    };
    
  

   
} // namespace lumiera
#endif
