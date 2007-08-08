/*
  PROCESSOR.hpp  -  a single render pipeline for one segment of the timeline
 
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


#ifndef PROC_ENGINE_PROCESSOR_H
#define PROC_ENGINE_PROCESSOR_H

#include "common/time.hpp"
#include "proc/stateproxy.hpp"




namespace proc
  {
  namespace engine
    {

    class ExitNode;

    class Processor
      {
      protected:
        ExitNode * output;

        /** begin of the timerange covered by this processor */
        cinelerra::Time start;

        /**end (exclusive) of the timerange  */
        cinelerra::Time end;

      };
      
      
      
  } // namespace proc::engine

} // namespace proc
#endif
