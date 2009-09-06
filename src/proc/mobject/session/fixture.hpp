/*
  FIXTURE.hpp  -  the (low level) representation of the EDL with explicit placement data
 
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


#ifndef MOBJECT_SESSION_FIXTURE_H
#define MOBJECT_SESSION_FIXTURE_H

#include <list>
#include <boost/scoped_ptr.hpp>

#include "proc/mobject/session/edl.hpp"
#include "proc/mobject/session/segmentation.hpp"
#include "proc/mobject/session/track.hpp"
#include "proc/mobject/explicitplacement.hpp"
#include "proc/mobject/session/auto.hpp"


using std::list;
using std::tr1::shared_ptr;



namespace mobject {
  namespace session {



    class Fixture : public EDL
      {
      protected:
        list<ExplicitPlacement> content_;
        boost::scoped_ptr<Segmentation> partitioning_;

        /////////////TODO: who creates this?

      public:
        list<ExplicitPlacement> & getPlaylistForRender () ;
        Auto<double>* getAutomation () ; ///< @todo: just a placeholder at the moment!!!
        
      private:
        virtual bool validate() 
          {
            TODO ("how to validate a Fixture?");
            return false;
          }

      };
    
    
    
    typedef shared_ptr<Fixture> PFix;



  } // namespace mobject::session

} // namespace mobject
#endif
