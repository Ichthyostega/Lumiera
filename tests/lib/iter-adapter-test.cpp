/*
  IterAdapter(Test)  -  building various custom iterators for a given container

  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>

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



#include "lib/test/run.hpp"
#include "lib/util.hpp"
#include "lib/util-foreach.hpp"

#include "lib/iter-adapter.hpp"

#include <boost/lexical_cast.hpp>
#include <iostream>
#include <vector>



namespace lib {
namespace test{
  
  using ::Test;
  using boost::lexical_cast;
  using util::for_each;
  using util::isnil;
  using std::vector;
  using std::cout;
  using std::endl;
  
  
  namespace {
  
    uint NUM_ELMS = 10;
    
    /** example of simply wrapping an STL container
     *  and exposing a range as Lumiera Forward Iterator
     */
    struct WrappedVector
      {
        vector<int> data_;
        
        WrappedVector(uint num)
          {
            while (num)
              data_.push_back(num--);
          }
        
        typedef vector<int>::iterator sourceIter;
        typedef RangeIter<sourceIter> iterator;
        
        typedef vector<int>::const_iterator const_sourceIter;
        typedef RangeIter<const_sourceIter> const_iterator;
        
        iterator       begin()       { return       iterator(data_.begin(),data_.end()); }
        iterator       end()         { return       iterator();                          }
        const_iterator begin() const { return const_iterator(data_.begin(),data_.end()); }
        const_iterator end()   const { return const_iterator();                          }
        
      };
    
    
    /** 
     * Example of a more elaborate custom container exposing an iteration API.
     * While the demo implementation here is based on pointers within a vector,
     * we hand out a IterAdapter, which will call back when used by the client,
     * thus allowing us to control the iteration process. Moreover, we provide
     * a variant of this iterator, which automatically dereferences the pointers,
     * thus yielding direct references for the client code to use.
     */
    class TestContainer
      {
        typedef vector<int *> _Vec;
        
        _Vec numberz_;
        
        static void killIt (int *it) { delete it; }
        
        
      public:
        TestContainer (uint count)
          : numberz_(count)
          {
            for (uint i=0; i<count; ++i)
              numberz_[i] = new int(i);
          }
        
       ~TestContainer ()
          {
            for_each (numberz_, killIt);
          }
        
        
        /* ==== Exposing Iterator interface(s) for the clients ====== */
        
        typedef IterAdapter<_Vec::iterator,       const TestContainer*> iterator;
        typedef IterAdapter<_Vec::const_iterator, const TestContainer*> const_iterator;
        
        typedef PtrDerefIter<iterator      > ref_iterator;
        typedef PtrDerefIter<const_iterator> const_ref_iter;
        
        
        iterator       begin ()           { return iterator       (this, numberz_.begin()); }
        const_iterator begin ()     const { return const_iterator (this, numberz_.begin()); }
        ref_iterator   begin_ref ()       { return ref_iterator   (begin()); }
        const_ref_iter begin_ref () const { return const_ref_iter (begin()); }
        
        iterator       end ()             { return iterator();       }
        const_iterator end ()       const { return const_iterator(); }
        
        
        
      protected: /* ==== API for the IterAdapter ==== */
        
        /** Implementation of Iteration-logic: pull next element. */
        template<class ITER>
        friend void
        iterNext (const TestContainer*, ITER& pos)
          {
            ++pos;
          }
        
        /** Implementation of Iteration-logic: detect iteration end.
         *  @note the problem here is that this implementation chooses
         *        to use two representations of "bottom" (end, invalid).
         *        The reason is, we want the default-constructed IterAdapter
         *        also be the "bottom" value. Thus, when we detect the
         *        iteration end by internal logic (\c numberz_.end() ), we
         *        immediately transform this into the official "bottom"
         */
        template<class ITER>
        friend bool
        hasNext (const TestContainer* src, ITER& pos)
          {
            REQUIRE (src);
            if ((pos != ITER(0)) && (pos != src->numberz_.end()))
              return true;
            else
              {
                pos = ITER(0);
                return false;
          }   }
      };
    
  } // (END) impl test dummy container
  
  
  
  
  
  
  
  /*********************************************************************
   * @test set up example implementations based on the iterator-adapter
   *       templates and verify the behaviour in accordance to the
   *       concept "lumiera forward iterator"
   *
   * @note see Ticket #182
   * @see IterAdapter
   * @see itertools.hpp
   * @see IterSource         
   */
  class IterAdapter_test : public Test
    {
      
      virtual void
      run (Arg arg)
        {
          if (0 < arg.size()) NUM_ELMS = lexical_cast<uint> (arg[0]);
          
          useSimpleWrappedContainer ();
          
          wrapIterRange ();
          TestContainer testElms (NUM_ELMS);
          simpleUsage (testElms);
          
          iterTypeVariations (testElms);
          verifyComparisons (testElms);
        }
      
      
      /** @test usage scenario, where we allow the client to
       *        access a range of elements given by STL iterators,
       *        without any specific iteration behaviour.
       */
      void
      wrapIterRange ()
        {
          vector<int> iVec (NUM_ELMS);
          for (uint i=0; i < NUM_ELMS; ++i)
            iVec[i] = i;
          
          typedef vector<int>::iterator I;
          typedef RangeIter<I> Range;
          
          Range range (iVec.begin(), iVec.end());
          CHECK (!isnil (range) || !NUM_ELMS);
          
          // now for example the client could....
          while ( range )
            cout << "::" << *range++;
          
          cout << endl;
          CHECK (isnil (range));
          CHECK (range == Range());
        }
      
      
      
      /** @test use the IterAdapter as if it was a STL iterator */
      template<class CON>
      void
      simpleUsage (CON& elms)
        {
          for_each (elms, showIntP);
          cout << endl;
        }
      
      static void showIntP (int* elm) { cout << "::" << *elm; }
      static void showInt  (int  elm) { cout << "::" <<  elm; }
      
      
      
      void
      useSimpleWrappedContainer ()
        {
          WrappedVector testVec (NUM_ELMS);
          for_each (testVec, showInt);
          cout << endl;
          
          WrappedVector const& ref (testVec);
          for_each (ref, showInt);  // uses const_iterator
          cout << endl;
        }
      
      
      /** @test verify the const and dereferencing variants,
       *        which can be created based on IterAdapter */
      void
      iterTypeVariations (TestContainer& elms)
        {
          TestContainer const& const_elms (elms);
          
          int i = 0;
          for (TestContainer::iterator iter = elms.begin();
               iter; ++iter, ++i
              )
            {
              CHECK (iter);
              CHECK (iter != elms.end());
              CHECK (**iter == i);
              --(**iter);
              CHECK (**iter == i-1);
            }
          
          i = 0;
          for (TestContainer::const_iterator iter = const_elms.begin();
               iter; ++iter, ++i
              )
            {
              CHECK (iter);
              CHECK (iter != elms.end());
              CHECK (**iter == i-1);
              
              // note: the previous run indeed modified
              // the element within the container.
              
            // ++(*iter);   // doesn't compile, because it yields a "* const"
            }
          
          i = 0;
          for (TestContainer::ref_iterator iter = elms.begin_ref();
               iter; ++iter, ++i
              )
            {
              CHECK (iter);
              CHECK ((*iter) == i-1);
              ++(*iter);
              CHECK ((*iter) == i);
            }
          
          i = 0;
          for (TestContainer::const_ref_iter iter = const_elms.begin_ref();
               iter; ++iter, ++i
              )
            {
              CHECK (iter);
              CHECK ((*iter) == i);
              
             // *iter = i+1;   ///////////TODO this should be const, but it isn't
            }
        }
      
      
      
      /** @test iterator comparison, predicates and operators */
      void
      verifyComparisons (TestContainer& elms)
        {
          TestContainer::ref_iterator rI (elms.begin_ref());
          
          CHECK (0 == *rI );
          CHECK (0 == *rI++);
          CHECK (1 == *rI  );
          CHECK (2 == *++rI);
          
          TestContainer const& const_elms (elms);
          TestContainer::const_ref_iter rI2 (const_elms.begin_ref());
          
          CHECK (rI2 != rI);
          CHECK (rI2 == elms.begin_ref());
          CHECK (rI2 == const_elms.begin_ref());
          
          ++++rI2;
          
          CHECK (rI2 == rI);
          CHECK (rI2 != ++rI);
          CHECK (!isnil (rI2));
          
          CHECK (TestContainer::iterator() == elms.end());
          CHECK (!(TestContainer::iterator()));
          CHECK (!(elms.end()));
          CHECK (isnil (elms.end()));
          
          CHECK (elms.begin());
          CHECK (!isnil (elms.begin()));
        }
      
      
    };
  
  LAUNCHER (IterAdapter_test, "unit common");
  
  
}} // namespace lib::test

