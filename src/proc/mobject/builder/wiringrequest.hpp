/*
  WIRINGREQUEST.hpp  -  intention to send the output of a pipe to another pipe's port
 
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


#ifndef MOBJECT_BUILDER_WIRINGREQUEST_H
#define MOBJECT_BUILDER_WIRINGREQUEST_H




namespace mobject {
  namespace builder {


    /**
     * Request to make a connection.
     * This connection may imply some data conversions,
     * thus it has to be \link #isInvestigated chcked \endlink
     * and may be deemed impossible. Otherwise, it can actually
     * be carried out by the help of the WiringMould. 
     * @see ToolFactory#provideMould(WiringRequest const&);
     */
    class WiringRequest
      {
      public:
        bool isInvestigated();
        bool isPossible();
      };



  } // namespace mobject::builder

} // namespace mobject
#endif
