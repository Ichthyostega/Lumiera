/*
  TESTDUMMY.hpp  -  yet another test dummy for tracking ctor/dtor calls
 
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
 
* *****************************************************/



#include <boost/noncopyable.hpp>


namespace lib {
namespace test{
    
  namespace { // yet another test dummy
    
    long checksum = 0;
    bool throw_in_ctor = false;
    
    class Dummy 
      : boost::noncopyable
      {
        int val_;
        
      public:
        Dummy ()
          : val_(1 + (rand() % 100000000))
          {
            checksum += val_;
            if (throw_in_ctor)
              throw val_;
          }
        
        ~Dummy()
          {
            checksum -= val_;
          }
        
        long add (int i)  { return val_+i; }
        
      protected:
        int getVal()  const { return val_; }
        
        void
        setVal (int newVal)
          {
            checksum += newVal - val_;
            val_ = newVal;
          }
      };
      
  }  // anonymous test dummy
  
}} // namespace lib::test

