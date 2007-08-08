/*
  BUILDABLE.hpp  -  marker interface denoting any (M)Object able to be treated by Tools
 
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


#ifndef PROC_MOBJECT_BUILDABLE_H
#define PROC_MOBJECT_BUILDABLE_H




namespace proc
  {
  namespace mobject
    {
    
    
    namespace builder{ class Tool; }
    
    
    /**
     *  All Buidables support double-dispatch of given Tool operations.
     *  The actual operation is thus selected at runtime based both on the 
     *  actual type of the Tool class /and/ the actual type of the Buildabele.
     */
    class Buildable
      {
      public:
        void apply (builder::Tool& provided_tool) ;
      };
      
      
      
  } // namespace proc::mobject

} // namespace proc
#endif
