/*
  STREAMTYPE.hpp  -  classification of media stream types 

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file streamtype.hpp
 ** Framework for classification of media streams.
 ** Besides the actual implementation type of a media stream, the Steam-Layer
 ** needs a more general way for accessing, comparing and manipulating media streams
 ** based on type information.
 ** 
 ** @see control::STypeManager
 ** 
 */


#ifndef STEAM_STREAMTYPE_H
#define STEAM_STREAMTYPE_H

#include "lib/symbol.hpp"
//#include "common/query.hpp"
#include "lib/idi/entry-id.hpp"



namespace steam {
  
  using lib::Symbol;

// "yes mummy, we all know this code is not finished yet..."
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuninitialized"
  
  /**
   * @todo this is just a draft to show the general idea....
   */
  struct StreamType
    : util::NonCopyable
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
          TRANSIENT
        };
      
      struct Prototype;
      
      class ImplFacade;
      class ImplConstraint;
      
      using ID = lib::idi::EntryID<StreamType>;
      
      
      Prototype const& prototype;
      ImplFacade * implType;       /////////////TODO: really by ptr???
      Usage intentionTag;
      
    };
#pragma GCC diagnostic pop
  
  
  /**
   * 
   */
  struct StreamType::Prototype
    {
      Symbol id;
      MediaKind kind;
      
      bool subsumes (Prototype const& other)  const;
      bool canRender (Prototype const& other)  const;
    };
  
  
  
  
  /**
   * A (more or less) concrete implementation type, wired up
   * as a facade providing the basic set of operations.
   */
  class StreamType::ImplFacade
    {
    public:
      Symbol libraryID;
      
      class TypeTag ; 
      
      /** 
       * placeholder type for the contents of a data buffer.
       * The actual buffer will always be provided by a 
       * library implementation; throughout the engine,
       * it's just hidden behind a DataBuffer pointer.
       */
      struct DataBuffer { };
      
      
      virtual bool operator== (ImplFacade const& other)  const =0;
      virtual bool operator== (StreamType const& other)  const =0;
      
      virtual bool canConvert (ImplFacade const& other)  const =0;
      virtual bool canConvert (StreamType const& other)  const =0;
      
      virtual DataBuffer* createFrame ()  const =0;
      virtual MediaKind getKind()         const =0;
      
      virtual ~ImplFacade() {};
      
    protected:
      ImplFacade (Symbol libID) ;
    };
  
  
  /**
   * Special case of an implementation type being only partially specified
   * Besides requiring some aspect of the implementation type, there is the
   * promise to fill in defaults to build a complete implementation type
   * if necessary.
   */
  class StreamType::ImplConstraint
    : public StreamType::ImplFacade
    {
    public:
      virtual bool canConvert (ImplFacade const& other)  const =0;
      virtual bool canConvert (StreamType const& other)  const =0;
      
      virtual bool subsumes (ImplFacade const& other)  const   =0;
      
      /** modify the other impl type such as to comply with this constraint */
      virtual void makeCompliant (ImplFacade & other)  const   =0;
      
      /** create a default impl type in accordance to this constraint
       *  and use it to create a new framebuffer */
      virtual DataBuffer* createFrame ()  const =0;
      
      /** Similarly create a impl type which complies to this constraint
       *  as well as to the additional constraints (e.g. frame size).
       *  Create a new frame buffer of the resulting type */
      virtual DataBuffer* createFrame (ImplConstraint const& furtherConstraints)  const =0;
      
      //TODO: do we need functions to represent and describe this constraint?
      
    };
  
  
    /** 
     * opaque placeholder (type erasure) 
     * for implementation specific type info.
     * Intended to be passed to a concrete
     * MediaImplLib to build an ImplFacade.
     */
  class StreamType::ImplFacade::TypeTag
    {
      void* rawTypeStruct_;
      
    public:
      Symbol libraryID;
      
      template<class TY>
      TypeTag (Symbol lID, TY& rawType)
        : rawTypeStruct_(&rawType),
          libraryID(lID)
        { }
    };
  
  
  
  
} // namespace steam


namespace lumiera {
  using steam::StreamType;
}

#endif /*STEAM_STREAMTYPE_H*/
