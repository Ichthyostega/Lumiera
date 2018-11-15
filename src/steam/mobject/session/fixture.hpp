/*
  FIXTURE.hpp  -  the (low level) representation of the Session with explicit placement data

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
 ** @todo WIP implementation of session core from 2010
 ** @todo as of 2016, this effort is considered stalled but basically valid
 */


#ifndef MOBJECT_SESSION_FIXTURE_H
#define MOBJECT_SESSION_FIXTURE_H

#include "steam/mobject/session/segmentation.hpp"
#include "steam/mobject/session/fork.hpp"
#include "steam/mobject/explicitplacement.hpp"
#include "steam/mobject/session/auto.hpp"
#include "lib/nocopy.hpp"

#include <memory>
#include <list>


using std::list;
using std::shared_ptr;
using std::unique_ptr;



namespace proc {
namespace mobject {
namespace session {
  
  
  
  /**
   * @todo 1/2012 Just a Placeholder. The real thing is not yet implemented.
   * @see http://lumiera.org/wiki/renderengine.html#Fixture
   */
  class Fixture
    : util::NonCopyable
    {
    protected:
      /////////////////////////////////////////////////TODO: placeholder code
      list<ExplicitPlacement> content_;
      unique_ptr<Segmentation> partitioning_;
      
      /////////////////////////////////////////////////TICKET #573  who creates the fixture?
      
    public:
      list<ExplicitPlacement> & getPlaylistForRender () ;
      Auto<double>* getAutomation () ; ///< @todo: just a placeholder at the moment!!!
      
      bool isValid()  const;
      
    private:
      virtual bool validate() 
        {
          TODO ("how to validate a Fixture?");
          return false;
        }
      
    };
  
  
  
  typedef shared_ptr<Fixture> PFix;
  
  
  
}}} // namespace proc::mobject::session
#endif
