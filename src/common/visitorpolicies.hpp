/*
  VISITOR.hpp  -  Acyclic Visitor library
 
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

====================================================================
This code is heavily inspired by  
 The Loki Library (loki-lib/trunk/include/loki/Visitor.h)
    Copyright (c) 2001 by Andrei Alexandrescu
    This Loki code accompanies the book:
    Alexandrescu, Andrei. "Modern C++ Design: Generic Programming
        and Design Patterns Applied". 
        Copyright (c) 2001. Addison-Wesley. ISBN 0201704315
 
*/



#ifndef CINELERRA_VISITORPOLICIES_H
#define CINELERRA_VISITORPOLICIES_H

#include "common/error.hpp"


namespace cinelerra
  {
  namespace visitor
    {
    /* == several Policies usable in conjunction with cinelerra::visitor::Visitable == */

    /** 
     * Policy returning just the default return value in case
     * of encountering an unknown Visitor (typically caused by
     * adding a new class to the visitable hierarchy)
     */
    template<class TOOL>
    struct UseDefault
      {
        typedef TOOL::ReturnType Ret;
        
        template<class TAR>
        static Ret onUnknown (TAR&, TOOL&)
          {
            return Ret();
          }
      };
      
    /** 
     * Policy to throw when encountering an unknown visiting tool
     */
    template<class TOOL>
    struct ThrowException
      {
        typedef TOOL::ReturnType Ret;
        
        template<class TAR>
        static Ret onUnknown (TAR&, TOOL&)
          {
            throw cinelerra::error::Config("unable to decide what tool operation to call");
          }
      };
      
    /** 
     * Policy invoking an catch-all function for processing
     * an unknown tool / target pair
     * @note using this policy effectively enforces 
     *       implementing a catch-all function \c treat(TAR&)
     */
    template<class TOOL>
    struct InvokeCatchAllFunction
      {
        typedef TOOL::ReturnType Ret;
        
        template<class TAR>
        static Ret onUnknown (TAR& target,TOOL& tool)
          {
            return tool.catchAll (target);
          }
      };
    
    
  } // namespace visitor

} // namespace cinelerra
#endif
