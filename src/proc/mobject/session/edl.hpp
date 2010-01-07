/*
  EDL.hpp  -  the (high level) Edit Decision List within the current Session
 
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


#ifndef MOBJECT_SESSION_EDL_H
#define MOBJECT_SESSION_EDL_H

#include <vector>
#include <string>

#include "proc/mobject/mobject.hpp"
#include "proc/mobject/placement.hpp"
#include "proc/mobject/session/track.hpp"

using proc_interface::PAsset;  // TODO better way to refer to a track?

using std::vector;
using std::string;

namespace mobject {
  namespace session {

    
    class EDL  ///////////////////////////TICKET #152  EDL will be renamed to "Sequence".
               ///////////////////////////TODO:  Should be converted to an facade interface
               ///////////////////////////TODO:  what follows is dummy code and will be dropped!
      {
      protected:
        Placement<Track>  track;
        vector<MObject *> clips;

      public:
        EDL();
        
        bool contains (const PMO& placement);
        PMO& find (const string& id); ///< @todo how to refer to clips? using asset IDs??
        
        Placement<Track>& getTracks () { return track; } ///< @todo work out the correct use of tracks! make const correct!
        size_t size ()
          {
            UNIMPLEMENTED ("what is the 'size' of an EDL?");
            return 0;
          }
        
        bool isValid() { return this->validate(); }
        
      private:
        virtual bool validate();
        
      };



  } // namespace mobject::session

} // namespace mobject
#endif
