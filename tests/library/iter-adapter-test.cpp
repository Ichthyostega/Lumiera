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

/** @file iter-adapter-test.cpp
 ** unit test \ref IterAdapter_test
 */



#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/util.hpp"
#include "lib/util-foreach.hpp"
#include "lib/format-cout.hpp"

#include "lib/iter-adapter.hpp"
#include "lib/iter-adapter-ptr-deref.hpp"

#include <boost/lexical_cast.hpp>
#include <vector>



namespace lib {
namespace test{
  
  using ::Test;
  using LERR_(ITER_EXHAUST);
  using boost::lexical_cast;
  using util::for_each;
  using util::isnil;
  using std::vector;
  
  
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
        
        size_t         size()  const { return numberz_.size(); }
        
        
        
      protected: /* ==== API for the IterAdapter ==== */
        
        /** Implementation of Iteration-logic: pull next element.
         * @remarks typically the implementation is simplistic,
         *        since the way this extension point is called from IterAdapter
         *        ensures that _`pos` is still valid_ and that the `checkPoint()` function
         *        is invoked immediately afterwards, allowing to adjust `pos` if necessary
         */
        template<class ITER>
        friend void
        iterNext (const TestContainer*, ITER& pos)
        {
          ++pos;
        }
        
        /** Implementation of Iteration-logic: detect iteration end.
         *  @note the problem here is that this implementation chooses to use
         *        two representations of ⟂ ("bottom", end, invalid). The reason is,
         *        we want the default-constructed IterAdapter also be the ⟂ value.
         *        This is in accordance with the »Lumiera Forward Iterator« concept,
         *        which requires the default constructed iterator to mark the iteration
         *        end and to evaluate to \c bool(false). Thus, when we detect the
         *        iteration end by internal logic (\c numberz_.end() ), we
         *        immediately transform this into the official "bottom"
         */
        template<class ITER>
        friend bool
        checkPoint (const TestContainer* src, ITER& pos)
        {
          REQUIRE (src);
          if ((pos != ITER()) && (pos != src->numberz_.end()))
            return true;
          else
            {
              pos = ITER();
              return false;
        }   }
      };
    
  } // (END) impl test dummy container
  
  
  
  
  
  
  
  /*****************************************************************//**
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
          if (0 < arg.size()) NUM_ELMS = lexical_cast<uint> (arg[1]);
          
          useSimpleWrappedContainer ();
          
          enumerate();
          wrapIterRange();
          TestContainer testElms (NUM_ELMS);
          simpleUsage (testElms);
          
          iterTypeVariations (testElms);
          verifyComparisons (testElms);
          exposeDataAddresses();
        }
      
      
      /** @test enumerate all number within a range */
      void
      enumerate()
        {
          long sum=0;
          const int N = NUM_ELMS;
          auto i = eachNum(1, N);
          while (i)
            {
              sum += *i;
              ++i;
            }
          
          CHECK (sum == (N-1)*N / 2);
          
          CHECK (!i);
          VERIFY_ERROR (ITER_EXHAUST, *i );
          VERIFY_ERROR (ITER_EXHAUST, ++i );
          
          i = eachNum (N, 2*N);
          CHECK (i);
          CHECK (N == *i);
          ++i;
          CHECK (N+1 == *i);
          for ( ; i; ++i)
            cout << "++" << *i;
          cout << endl;
          
          CHECK (!i);
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
            {
              cout << "::" << *range;
              ++range;
            }
          
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
          
          
          //---- verify support for C++11 element iteration
          i = 0;
          for (auto& elm : elms) // NOTE: TestContainer exposes pointers
            {
              ++elm; // can indeed modify contents
              --elm;
              CHECK (*elm == i);
              ++i;
            }
          CHECK (size_t(i) == elms.size());
          
          i = 0;
          for (auto const& elm : elms)
            {
              CHECK (*elm == i);
              // ++elm; // can not modify contents
              ++i;
            }
          CHECK (size_t(i) == elms.size());
          
          i = 0;
          for (auto const& elm : const_elms)
            {
              CHECK (*elm == i);
              // ++elm; // can not modify contents
              ++i;
            }
          CHECK (size_t(i) == elms.size());
        }
      
      
      /** @test build an iterator to expose
       *        the address of underlying data elements */
      void
      exposeDataAddresses()
        {
          vector<int> numbz;
          for (uint i=0; i < NUM_ELMS; ++i)
            numbz.push_back(i);
          
          typedef vector<int>::iterator      RawIter;
          typedef RangeIter<RawIter>         Range;
          typedef AddressExposingIter<Range> AddrIter;
          
          AddrIter ii(Range(numbz.begin(), numbz.end()));
          for (uint i=0; i < numbz.size(); ++i)
            {
              CHECK (ii);
              int* p = *ii;
              CHECK (p == & numbz[i]);
              ++ii;
            }
          CHECK (!ii);
          
          // building a const iterator needs to be done in a somewhat weird way;
          // since we're exposing the pointer as value, the solution is to add
          // the const on the immediately wrapped iterator type 
          typedef vector<int>::const_iterator     ConstRawIter;
          typedef RangeIter<ConstRawIter>         ConstRange;
          typedef AddressExposingIter<ConstRange> ConstAddrIter;
          
          ConstAddrIter iic(ConstRange(Range(numbz.begin(), numbz.end())));
          for (uint i=0; i < numbz.size(); ++i)
            {
              CHECK (iic);
              const int* p = *iic;
              CHECK (p == & numbz[i]);
              ++iic;
            }
          CHECK (!iic);
        }
      
      
      
      
      /** @test iterator comparison, predicates and operators */
      void
      verifyComparisons (TestContainer& elms)
        {
          TestContainer::ref_iterator rI (elms.begin_ref());
          
          CHECK (0 == *rI );
          ++rI;
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

