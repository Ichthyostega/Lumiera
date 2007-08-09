/*
  PROCNODE.hpp  -  Key abstraction of the Render Engine: a Processing Node
 
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


#ifndef ENGINE_PROCNODE_H
#define ENGINE_PROCNODE_H

#include <vector>

#include "proc/mobject/parameter.hpp"


using std::vector;



namespace engine
  {


  /**
   * Key abstraction of the Render Engine: A Data processing Node
   */
  class ProcNode
    {
    protected:
      typedef mobject::Parameter<double> Param;

      /** The predecessor in a processing pipeline.
       *  I.e. a source to get data to be processed
       */
      ProcNode * datasrc;

      vector<Param> params;

    };

} // namespace engine
#endif
