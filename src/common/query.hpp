/*
  QUERY.hpp  -  interface for capability queries
 
  Copyright (C)         CinelerraCV
    2007,               Hermann Vosseler <Ichthyostega@web.de>
 
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


#ifndef CINELERRA_QUERY_H
#define CINELERRA_QUERY_H


#include <string>



namespace cinelerra
  {
  using std::string;
  
  /** 
   * Generic query interface for retrieving objects matching
   * some capability query
   */
  template<class STRU>
  class Query : public std::string
    {
    public:
      Query (string predicate="") : string(predicate) {}
    };

    
  namespace query
    {
    
    /** ensure standard format for a given id string.
     *  Trim, sanitize and ensure the first letter is lower case.
     *  @note modifies the given string ref in place
     */
    void normalizeID (string& id);
    
  
  } // namespace query
    
} // namespace cinelerra
#endif
