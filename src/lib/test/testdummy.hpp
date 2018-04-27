/*
  TESTDUMMY.hpp  -  yet another test dummy for tracking ctor/dtor calls

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of
  the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

* *****************************************************/


/** @file testdummy.hpp
 ** unittest helper code: test dummy object.
 ** This dummy includes some distinct random identity marker
 ** plus a checksum facility to verify instance management, i.e.
 ** to verify each created instance was properly destroyed after use.
 */


#include "lib/nocopy.hpp"

#include <algorithm>


namespace lib {
namespace test{
    
  
  class Dummy 
    : util::NonCopyable
    {
      int val_;
      
      /** to verify ctor/dtor calls */
      static long _local_checksum;
      static bool _throw_in_ctor;
      
    public:
      Dummy ()
        : val_(1 + (rand() % 100000000))
        { init(); }
      
      Dummy (int v)
        : val_(v)
        { init(); }
      
      virtual ~Dummy()
        {
          checksum() -= val_;
        }
      
      virtual long
      acc (int i)   ///< dummy API operation
        {
          return val_+i;
        }
      
      int
      getVal()  const
        {
          return val_;
        }
      
      void
      setVal (int newVal)
        {
          checksum() += newVal - val_;
          val_ = newVal;
        }
      
      friend void
      swap (Dummy& dum1, Dummy& dum2)  ///< checksum neutral
      {
        std::swap(dum1.val_, dum2.val_);
      }
      
      static long&
      checksum()
        {
          return _local_checksum;
        }
      
      static void
      activateCtorFailure(bool indeed =true)
        {
          _throw_in_ctor = indeed;
        }
      
      
    private:
      void
      init()
        {
          checksum() += val_;
          if (_throw_in_ctor)
            throw val_;
        }

    };
  
  
}} // namespace lib::test

