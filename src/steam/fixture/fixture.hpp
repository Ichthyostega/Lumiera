/*
  FIXTURE.hpp  -  the (low level) representation of the Session with explicit placement data

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file fixture.hpp
 ** Backbone data structure of the low-level render node model
 ** The fixture defines the boundary between the Session (high-level) realm
 ** and the internals of the render engine. The goal of a Builder run is to
 ** build a new Fixture. All relative or indirect referrals are resolved at that point
 ** and all time positions or output designations are made explicit. The Fixture defines
 ** a Segmentation of every (top-level) timeline, and thus defines those segments which
 ** can be rendered with a single wiring configuration. This Segmentation, as defined as
 ** part of the Fixture, is also the foundation for memory management within the engine
 ** model, since the allocation of render nodes for a given segment happens at once, and
 ** segments are obliterated as a whole, when being replaced by a new version as result
 ** of a more recent builder run. Ongoing render processes are also tracked per segment,
 ** which allows the individual calculation steps just to assume the data is "there".
 ** 
 ** @ingroup fixture
 ** 
 ** @todo WIP implementation of session core from 2010
 ** @todo as of 2016, this effort is considered stalled but basically valid
 */


#ifndef STEAM_FIXTURE_FIXTURE_H
#define STEAM_FIXTURE_FIXTURE_H

#include "steam/fixture/segmentation.hpp"
#include "steam/mobject/session/fork.hpp"
#include "steam/mobject/explicitplacement.hpp"
#include "steam/mobject/session/auto.hpp"
#include "lib/nocopy.hpp"

#include <memory>
#include <list>


using std::list;
using std::shared_ptr;
using std::unique_ptr;



namespace steam {
namespace fixture {
  
  using mobject::session::Auto;
  using mobject::ExplicitPlacement;
  
  
  /**
   * @ingroup fixture
   * @todo 1/2012 Just a Placeholder. The real thing is not yet implemented.
   * @see http://lumiera.org/wiki/renderengine.html#Fixture
   */
  class Fixture
    : util::NonCopyable
    {
    protected:
      ///////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #725 : placeholder code
      list<ExplicitPlacement> content_;
      unique_ptr<Segmentation> partitioning_;
      
      ///////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #573  who creates the fixture?
      
    public:
      Auto<double>* getAutomation () ; ///< @todo: just a placeholder at the moment!!!
      
      bool isValid()  const;
      
    private:
      virtual bool validate()
        {
          TODO ("how to validate a Fixture?");
          return false;
        }
      
    };
  
  using PFixture = shared_ptr<Fixture>;
  
  
  
}} // namespace steam::fixture
#endif /*STEAM_FIXTURE_FIXTURE_H*/
