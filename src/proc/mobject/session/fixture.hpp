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


#ifndef MOBJECT_SESSION_FIXTURE_H
#define MOBJECT_SESSION_FIXTURE_H

#include "proc/mobject/session/segmentation.hpp"
#include "proc/mobject/session/fork.hpp"
#include "proc/mobject/explicitplacement.hpp"
#include "proc/mobject/session/auto.hpp"

#include <boost/noncopyable.hpp>
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
    : boost::noncopyable
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
