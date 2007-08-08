/*
  TRACK.hpp  -  descriptor for one track in the Session
 
  Copyright (C)         CinelerraCV
    2007,               Christian Thaeter <ct@pipapo.org>
 
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


#ifndef PROC_MOBJECT_SESSION_TRACK_H
#define PROC_MOBJECT_SESSION_TRACK_H



namespace proc
  {
  namespace mobject
    {
    namespace session
      {


      /**
       * A Track in the EDL or Session. 
       * But, honestly, I don't quite know what a Track stands for!
       * Usually, this stems from the metaphor of a multitrack tape machine,
       * but I doubt this metaphor is really helpful for editing video; mostly
       * people stick to such metaphors out of mental laziyness...
       * 
       * So, let's see if the concept "Track" will get any practical functionallity
       * or if we end up with "Track" beeing just a disguise for an int ID.... 
       */
      class Track
        {
          
        };

        
        
    } // namespace proc::mobject::session

  } // namespace proc::mobject

} // namespace proc
#endif
