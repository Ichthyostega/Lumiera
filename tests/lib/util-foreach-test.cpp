/*
  UtilForeach(Test)  -  helpers for doing something for each element
 
  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>
 
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


#include "lib/test/run.hpp"
#include "lib/util.hpp"
#include "lib/iter-adapter.hpp"


#include <boost/lexical_cast.hpp>
#include <boost/lambda/lambda.hpp>
#include <tr1/functional>
#include <iostream>
#include <vector>


using ::Test;

using util::for_each;
using util::has_any;
using util::and_all;

using boost::lexical_cast;
using std::tr1::function;
using std::tr1::ref;
using std::cout;
using std::endl;

using namespace boost::lambda;


namespace util {
namespace test {
  
  typedef std::vector<int> VecI;
  typedef lib::RangeIter<VecI::iterator> RangeI;
  
  
  
  namespace{ // Test data and operations
    
    uint NUM_ELMS = 10;
    
    
    // need explicit definitions here, because we use
    // tr1/functional and boost::lambda at the same time
    std::tr1::_Placeholder<1>        _1;
    boost::lambda::placeholder1_type _1_;
    
    
    VecI
    buildTestNumberz (uint count)
    {
      VecI numbers;
      numbers.reserve(count);
      while (count)
        numbers.push_back(count--);
      
      return numbers;
    }
    
    
    /* == functions to bind and invoke == */
    bool
    plainFunc (int i)
    {
      cout <<':'<< i;
      return i;
    }
    
    bool
    function1 (int i, int j)
    {
      return plainFunc(i+j);
    }
    
    bool
    function2 (int i, int j, int& k)
    {
      k = i + j;
      return plainFunc(k);
    }
    
    
#define _NL_ cout << endl;
    
  } // (End) test data and operations
  
  
  
  /*********************************************************************
   * @test Invoking an operation for each element of a collection.
   *       Covers the various flavours of these convenience helpers:
   *       They might operate either on a STL container (providing 
   *       \c begin() and \c end() functions), or at a "Lumiera
   *       Forward Iterator", which is incremented and dereferenced
   *       for each value it yields, until exhaustion.
   *       
   *       Moreover for each of these cases, there are additional
   *       overloads allowing to create a bind-expression on-the fly.
   *       As a further variation of this scheme, an predicate can be
   *       evaluated for each element, either with universal quantisation 
   *       (results are && combined), or with existential quantisation.
   */
  class UtilForeach_test : public Test
    {
      
      void
      run (Arg arg)
        {
          if (0 < arg.size()) NUM_ELMS = lexical_cast<uint> (arg[0]);
          
          VecI container = buildTestNumberz (NUM_ELMS);
          RangeI iterator(container.begin(), container.end());
          
          check_foreach_plain (container);
          check_foreach_plain (iterator);
          
          check_foreach_bind (container);
          check_foreach_bind (iterator);
          
          check_foreach_memFun (container);
          check_foreach_memFun (iterator);
          
          check_foreach_lambda (container);
          check_foreach_lambda (iterator);
          
          check_existence_quant (container);
          check_existence_quant (iterator);
          
          check_invoke_on_each (iterator);
        }
      
      
      /** @test invoke a simple free function, given
       *        as reference, function pointer or functor.
       *        The invoked test function will print its argument
       */
      template<typename CO>
      void
      check_foreach_plain (CO coll)
        {
          function<bool(int)> func(plainFunc);
          
          for_each (coll, plainFunc);      _NL_
          for_each (coll, &plainFunc);     _NL_
          for_each (coll, func);           _NL_
          
          and_all (coll, plainFunc);       _NL_
          and_all (coll, &plainFunc);      _NL_
          and_all (coll, func);            _NL_
          
          has_any (coll, plainFunc);       _NL_
          has_any (coll, &plainFunc);      _NL_
          has_any (coll, func);            _NL_
        }
      
      
      /** @test bind additional parameters on-the-fly,
       *        including the possibility to use a placeholder
       *        to denote the position of the variable parameter */
      template<typename CO>
      void
      check_foreach_bind (CO coll)
        {
          function<bool(int,int)>     fun1(function1);
          function<bool(int,int,int)> fun2(function2);
          
          for_each (coll, function1, 10, _1 );               _NL_
          for_each (coll, &function1,10, _1 );               _NL_
          for_each (coll, fun1,      10, _1 );               _NL_
          
          and_all (coll, function1,  10, _1 );               _NL_
          and_all (coll, &function1, 10, _1 );               _NL_
          and_all (coll, fun1,       10, _1 );               _NL_
          
          has_any (coll, function1,  10, _1 );               _NL_
          has_any (coll, &function1, 10, _1 );               _NL_
          has_any (coll, fun1,       10, _1 );               _NL_
          
          for_each (coll, function1, _1, _1 );               _NL_
          for_each (coll, &function1,_1, _1 );               _NL_
          for_each (coll, fun1,      _1, _1 );               _NL_
          
          and_all (coll, function1,  _1, _1 );               _NL_
          and_all (coll, &function1, _1, _1 );               _NL_
          and_all (coll, fun1,       _1, _1 );               _NL_
          
          has_any (coll, function1,  _1, _1 );               _NL_
          has_any (coll, &function1, _1, _1 );               _NL_
          has_any (coll, fun1,       _1, _1 );               _NL_
          
          // does not compile:
          for_each (coll, function1, 10, 20, _1 );
          for_each (coll, function1, 10, 20 );
          for_each (coll, function1, 10 );
          
          
          for_each (coll, function2, 10, 20, _1 );           _NL_
          for_each (coll, &function2,10, 20, _1 );           _NL_
          for_each (coll, fun2,      10, 20, _1 );           _NL_
          
          and_all (coll, function2,  10, 20, _1 );           _NL_
          and_all (coll, &function2, 10, 20, _1 );           _NL_
          and_all (coll, fun2,       10, 20, _1 );           _NL_
          
          has_any (coll, function2,  10, 20, _1 );           _NL_
          has_any (coll, &function2, 10, 20, _1 );           _NL_
          has_any (coll, fun2,       10, 20, _1 );           _NL_
          
          int sum=0;
          
          for_each (coll, function2, _1, _1, ref(sum) );     _NL_
          for_each (coll, &function2,_1, _1, ref(sum) );     _NL_
          for_each (coll, fun2,      _1, _1, ref(sum) );     _NL_
          
          and_all (coll, function2,  _1, _1, ref(sum) );     _NL_
          and_all (coll, &function2, _1, _1, ref(sum) );     _NL_
          and_all (coll, fun2,       _1, _1, ref(sum) );     _NL_
          
          has_any (coll, function2,  _1, _1, ref(sum) );     _NL_
          has_any (coll, &function2, _1, _1, ref(sum) );     _NL_
          has_any (coll, fun2,       _1, _1, ref(sum) );     _NL_
          
        }
      
      
      
      struct Dummy
        {
          int sum_;
          
          bool
          fun (int i)
            {
              sum_ += i;
              return plainFunc (sum_);
            }
        };
      
      /** @test bind a member function to be invoked for each element */
      template<typename CO>
      void
      check_foreach_memFun (CO coll)
        {
          Dummy dummy;
          
          for_each (coll, &Dummy::fun, dummy,  _1 );      _NL_
          and_all  (coll, &Dummy::fun, dummy,  _1 );      _NL_
          has_any  (coll, &Dummy::fun, dummy,  _1 );      _NL_
          
          for_each (coll, &Dummy::fun, &dummy, _1 );     _NL_
          and_all  (coll, &Dummy::fun, &dummy, _1 );     _NL_
          has_any  (coll, &Dummy::fun, &dummy, _1 );     _NL_
          
          
        }
      
      
      /** @test use a lambda-expression, to be invoked for each element */
      template<typename CO>
      void
      check_foreach_lambda (CO coll)
        {
          uint sum;
          for_each (coll, _1_ + var(sum));
          
          ASSERT (sum == NUM_ELMS/2 * (NUM_ELMS+1));
          
          ASSERT (!and_all  (coll, _1_ - 1 ));
          ASSERT ( has_any  (coll, _1_ + 1 ));
        }
      
      
      /** @test verify the logic of universal and existential quantisation.
       *        We use a predicate generated on-the-fly as lambda expression */
      template<typename CO>
      void
      check_existence_quant (CO coll)
        {
          ASSERT ( and_all (coll, 0 < _1_ ));
          ASSERT (!and_all (coll, 1 < _1_ ));
          
          ASSERT ( has_any (coll, 0 < _1_ ));
          ASSERT ( has_any (coll, _1_ >= NUM_ELMS ));
          ASSERT (!has_any (coll, _1_ >  NUM_ELMS ));
        }
      
      
      struct TestElm
        {
          uint n_;
          TestElm(uint i) : n_(i) {}
          
          void operation() { plainFunc (n_); }
        };
      
      
      /** @test the binding can also be used to \em dispatch an operation
       *        on each element within a object collection: here the parameter
       *        is used as \c this pointer to specify the object instance */
      template<typename CO>
      void
      check_invoke_on_each (CO coll)
        {
          vector<TestElm> elms;
          for (uint i=0; i<6; ++i)
            elms.push_back (TestElm(i));
          
          vector<TestElm*> elmPtrs;
          for (uint i=0; i<6; ++i)
            elms.push_back (& elms[i]);
          
          // fed the element pointer as "this" pointer of the member function
          for_each (elmPtrs, &TestElm::operation, _1 );      _NL_
          and_all  (elmPtrs, &TestElm::operation, _1 );      _NL_
          has_any  (elmPtrs, &TestElm::operation, _1 );      _NL_
          
          // but works with references as well
          for_each (elms, &TestElm::operation, _1 );         _NL_
          and_all  (elms, &TestElm::operation, _1 );         _NL_
          has_any  (elms, &TestElm::operation, _1 );         _NL_
        }
    };
  
  
  
  
  LAUNCHER (UtilForeach_test, "unit common");
  
  
}} // namespace util::test

