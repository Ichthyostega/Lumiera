/*
  META.hpp  -  abstract base class of all MObjects representing meta data or processing instructions
 
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


#ifndef PROC_MOBJECT_SESSION_META_H
#define PROC_MOBJECT_SESSION_META_H

#include "proc/mobject/session/abstractmo.hpp"



namespace proc
  {
  namespace mobject
    {
    namespace session
      {


      /** 
       * Meta-MObject doesn't represent real Media Content,
       * but rather all sorts of Processing Instructions
       * and other metadata, which can be placed and
       * attached within the EDL/Session.
       */
      class Meta : public AbstractMO
        {
          ///////////
        };
        
        
        
    } // namespace proc::mobject::session

  } // namespace proc::mobject

} // namespace proc
#endif
