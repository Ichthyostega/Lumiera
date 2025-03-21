/*
  ABSTRACTMO.hpp  -  abstract base class for all Media Objects

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file abstractmo.hpp
 ** @todo result of the very first code generation from UML in 2008. Relevance not clear yet... 
 */


#ifndef MOBJECT_SESSION_ABSTRACTMO_H
#define MOBJECT_SESSION_ABSTRACTMO_H

#include "steam/mobject/mobject.hpp"
#include "lib/symbol.hpp"



namespace steam {
namespace mobject {
namespace session {
  
  namespace error = lumiera::error;
  
  
  /**
   *  abstract base class of all MObjects for providing common services.
   *  @todo seems that we don't need this intermediate class...
   */
  class AbstractMO
    : public MObject
    {
      
     
      /* === some default implementations ===  */
      
      DEFINE_PROCESSABLE_BY (builder::BuilderTool);
      
      string
      initShortID()  const
        {
          return buildShortID("MObject");
        }
      
    public:
      
      Duration&
      getLength()
        {
          return length_; 
        }
      
      bool operator== (const MObject& oo)  const;
      
    protected:
      void
      throwIfInvalid()  const
        {
          if (!isValid())
            throw error::Fatal ("Invalid MObject in model. Indicates a race "
                                "or similarly broken internal assumptions.");
        }
      
      string buildShortID (lib::Literal typeID, string suffix ="")  const;
    };
  
  
  
}}} // namespace steam::mobject::session
#endif
