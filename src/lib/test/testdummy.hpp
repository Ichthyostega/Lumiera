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
 ** unittest helper code: test dummy objects to track instances.
 ** These can be used to verify proper allocation handling, either by
 ** watching the checksum of \ref Dummy, or by matching on the \ref EventLog
 ** embedded into \ref Tracker.
 */


#include "lib/nocopy.hpp"
#include "lib/test/event-log.hpp"
#include "lib/format-string.hpp"
#include "lib/util.hpp"

#include <algorithm>
#include <limits>


namespace lib {
namespace test{
  
  
  /**
   * A Dummy object for tests. Each instance includes some distinct
   * random identity marker plus a checksum facility to verify instance management,
   * i.e. to verify each created instance was properly destroyed after use.
   */
  class Dummy
    : util::MoveAssign
    {
      int val_;
      
      /** to verify ctor/dtor calls */
      static long _local_checksum;
      static bool _throw_in_ctor;
      
    public:
      virtual ~Dummy()       ///< can act as interface
        {
          checksum() -= val_;
        }
      
      Dummy ()
        : val_(1 + (rand() % 100000000))
        { init(); }
      
      Dummy (int v)
        : val_(v)
        { init(); }
      
      Dummy (Dummy && oDummy)
        : Dummy(0)
        {
          swap (*this, oDummy);
        }
      
      Dummy&
      operator= (Dummy && oDummy)
        {
          if (&oDummy != this)
            swap (*this, oDummy);
          return *this;
        }
      
      
      /** a dummy API operation */
      virtual long
      acc (int i)
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
        std::swap (dum1.val_, dum2.val_);
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

  
  
  
  /**
   * A tracking Dummy object for tests.
   * All construction-, destruction and copy operations
   * are logged into an embedded lib::test::EventLog
   */
  struct Tracker
    {
      static lib::test::EventLog log;
      
      static constexpr int DEFUNCT = std::numeric_limits<int>::min();
      static constexpr int DEAD    = std::numeric_limits<int>::max();
      
      int val;
      
     ~Tracker()
        {
          log.call (this,"dtor", val);
          val = DEAD;
        }
      
      Tracker()
        : val{rand() % 1000}
        {
          log.call (this,"ctor");
        }
      
      Tracker(int v)
        : val{v}
        {
          log.call (this,"ctor", v);
        }
      
      Tracker(Tracker const& ol)
        : val{ol.val}
        {
          log.call (this,"ctor-copy", ol);
        }
      
      Tracker(Tracker && oo)
        : val{oo.val}
        {
          log.call (this,"ctor-move", oo);
          oo.val = DEFUNCT;
        }
      
      Tracker&
      operator= (Tracker const& ol)
        {
          if (util::isSameObject (*this, ol))
            {
              log.call (this, "self-assign-copy");
            }
          else
            {
              log.call (this, "assign-copy", ol);
              val = ol.val;
            }
          return *this;
        }
      
      Tracker&
      operator= (Tracker && oo)
        {
          if (util::isSameObject (*this, oo))
            {
              log.call (this, "self-assign-move");
            }
          else
            {
              log.call (this, "assign-move", oo);
              val = oo.val;
              oo.val = DEFUNCT;
            }
          return *this;
        }
      
      operator string()  const
        {
          return util::_Fmt{"Track{%02d}"} % val;
        }
      
      friend void
      swap (Tracker& t1, Tracker& t2)
      {
        log.call ("static", "swap", t1, t2);
        std::swap (t1.val, t2.val);
      }
    };
  
  
  
}} // namespace lib::test

