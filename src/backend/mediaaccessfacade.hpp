/*
  MEDIAACCESSFACADE.hpp  -  functions for querying media file and channels.
 
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


#ifndef BACKEND_INTERFACE_MEDIAACCESSFACADE_H
#define BACKEND_INTERFACE_MEDIAACCESSFACADE_H


#include "common/singleton.hpp"



namespace backend_interface
  {


  /**
   * Interface to the backend layer:
   * provides functions for querying (opening) a media file,
   * detecting the channels or streams found within this file etc. 
   * Implemention delegating to the actual backend functions.
   */
  class MediaAccessFacade
    {
    public:
      static Singleton<MediaAccessFacade> instance;
      
      
    };
  
  
  
} // namespace backend_interface
#endif
