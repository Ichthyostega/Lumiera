/*
  TYPELIST.hpp  -  typelist meta programming facilities
 
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

====================================================================
This code is heavily inspired by  
 The Loki Library (loki-lib/trunk/include/loki/Sequence.h)
    Copyright (c) 2001 by Andrei Alexandrescu
    Copyright (c) 2005 by Peter Kümmel
    This Loki code accompanies the book:
    Alexandrescu, Andrei. "Modern C++ Design: Generic Programming
        and Design Patterns Applied". 
        Copyright (c) 2001. Addison-Wesley. ISBN 0201704315
        
  Loki Copyright Notice: 
  Permission to use, copy, modify, distribute and sell this software for any
  purpose is hereby granted without fee, provided that the above copyright
  notice appear in all copies and that both that copyright notice and this
  permission notice appear in supporting documentation.
  The author makes no representations about the suitability of this software
  for any purpose. It is provided "as is" without express or implied warranty.
*/


/** @file typelist.hpp
 ** A template metaprogramming technique for manipulating collections of types.
 ** Effectively this is a taylored and simplified version of what can be found in the Loki library.
 ** We use it in other generic library-style code to generate repetitive code. If you tend to find
 ** template metaprogramming (or functional programming in general) offending, please ignore the 
 ** technical details and just consider the benefit of such an simplification for the user code.
 **
 ** Interface for using this facility is the template Types(.....) for up to 20 Type parameters
 **
 ** @see lumiera::visitor::Applicable usage example
 ** @see typelisttest.cpp
 ** 
 */


#ifndef LUMIERA_TYPELIST_H
#define LUMIERA_TYPELIST_H




namespace lumiera
  {
  namespace typelist
    {
    
    class NullType { };
    
    template<class H, class T> 
    struct Node
      {
        typedef H Head;
        typedef T Tail;
      };
    
    template
      < class T01=NullType
      , class T02=NullType
      , class T03=NullType
      , class T04=NullType
      , class T05=NullType
      , class T06=NullType
      , class T07=NullType
      , class T08=NullType
      , class T09=NullType
      , class T10=NullType
      , class T11=NullType
      , class T12=NullType
      , class T13=NullType
      , class T14=NullType
      , class T15=NullType
      , class T16=NullType
      , class T17=NullType
      , class T18=NullType
      , class T19=NullType
      , class T20=NullType
      >
    class Types
      {
        typedef typename Types<      T02, T03, T04
                              , T05, T06, T07, T08
                              , T09, T10, T11, T12
                              , T13, T14, T15, T16
                              , T17, T18, T19, T20>::List ListTail;
      public:
        typedef Node<T01, ListTail> List;
      };
    
    template<> 
    struct Types<>
      {
        typedef NullType List;
      };
    
    
    
  } // namespace typelist

} // namespace lumiera
#endif
