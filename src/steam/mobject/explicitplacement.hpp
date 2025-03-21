/*
  EXPLICITPLACEMENT.hpp  -  special Placement yielding an absolute location (Time,Output) for a MObject

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file explicitplacement.hpp
 ** Core abstraction: completely resolved placement of an MObject
 ** Within the session model, all media objects are attached with the help
 ** of mobject::Placement elements. These are the "glue" to stitch the model
 ** together. However, placements are typically only defined in parts, and what
 ** is left out in the definition is assumed to be _"obvious from the context"._
 ** 
 ** The core operation within Steam-Layer is the Builder run, which walks the Session model
 ** to resolve and fill in all the contextual information. The result is a completely resolved
 ** Placement for each actually visible and relevant entity. Such is represented as ExplicitPlacement.
 ** These are arranged into the backbone structure, the Fixture, and the actual render node network
 ** necessary to _render_ those contents is attached below.
 */


#ifndef STEAM_MOBJECT_EXPLICITPLACEMENT_H
#define STEAM_MOBJECT_EXPLICITPLACEMENT_H

#include "steam/mobject/placement.hpp"



namespace steam {
namespace mobject {
  
  
  
  /**
   * Special kind of Placement, where the location of the
   * MObject has been nailed down to a fixed position.
   * The Session maintains a special list of Placements,
   * which actually is a snapshot of all Session contents fixed
   * and reduced to simple positions. This so called Fixture
   * contains only ExplicitPlacement objects and is processed
   * by the Builder to create the render engine  node network.
   *
   * @ingroup fixture
   * @see Placement#resolve factory method for deriving an ExplicitPlacement 
   */
  class ExplicitPlacement : public Placement<MObject>
    {
    public:
      const Time time;
      const Pipe pipe;
      
      typedef std::pair<Time,Pipe> SolutionData;  //TODO (ichthyo considers better passing of solution by subclass)
      
      /** no need to resolve any further, as this ExplicitPlacement
       *  already \e is the result of a resolve()-call.
       *                                           //////////////////////TICKET #439
       */
      virtual
      ExplicitPlacement resolve ()  const 
        { 
          return *this; 
        }
      
      
    protected:
      /*  @todo ichthyo considers a much more elegant implementation utilising a subclass 
       *        of FixedLocation, which would serve as Placement::LocatingSolution, and
       *        would be used as LocatingPin::chain subobject as well, so that it could
       *        be initialised directly here in the ExplicitPlacement ctor. 
       *        (ichthyo: see Trac #100)
       */
      ExplicitPlacement (const Placement<MObject>& base, const SolutionData found)
        : Placement<MObject>(base),
          time(found.first), pipe(found.second)
        { };
        
      friend ExplicitPlacement Placement<MObject>::resolve () const;
      
    private:
      /** copying prohibited, ExplicitPlacement is effectively const! */
      ExplicitPlacement& operator= (const ExplicitPlacement&);
    };
  
  
  
}} // namespace steam::mobject
#endif /*STEAM_MOBJECT_EXPLICITPLACEMENT_H*/
