/*
  TEST-TARGET-OBJ.hpp  -  a test (stub) target object for testing the factories

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file test-target-obj.hpp
 ** Dummy target object to be created by factory for unit tests
 ** Used to verify sane memory management and instance lifecycle for such
 ** objects generated as singleton or by factory
 */


#ifndef LIBRARY_TEST_TARGET_OBJ_H
#define LIBRARY_TEST_TARGET_OBJ_H


#include "lib/test/run.hpp"
#include "lib/format-string.hpp"
#include "lib/format-cout.hpp"

#include <boost/lexical_cast.hpp>
#include <string>


namespace lib {
namespace test{

  using util::_Fmt;
  using boost::lexical_cast;
  using std::string;
  
  /**
   * Target object to be created by Test-Factories or as Singleton.
   * Allocates a variable amount of additional heap memory
   * and prints diagnostic messages.
   */
  class TestTargetObj
    {
      uint cnt_;
      string* heapData_;
      string* heapArray_;
      
    public:
      TestTargetObj(uint num);
      virtual ~TestTargetObj();
      
      virtual operator string () const;
    };
  
  
  
  inline
  TestTargetObj::TestTargetObj(uint num)
    : cnt_ (num)
    , heapData_ (new string(num,'*'))
    , heapArray_ (new string[num])
    {
      for (uint i=0; i<cnt_; ++i)
        heapArray_[i] = lexical_cast<string>(i);
      cout << _Fmt("ctor TargetObj(%i) successful\n") % cnt_;
    }
  
  
  inline
  TestTargetObj::~TestTargetObj()   ///< EX_FREE
  {
    delete heapData_;
    delete[] heapArray_;
    cout << _Fmt("dtor ~TargetObj(%i) successful\n") % cnt_;
  }
  
  
  
  inline
  TestTargetObj::operator string () const
  {
    string array_contents = "{";
    for (uint i=0; i<cnt_; ++i)
      array_contents += heapArray_[i]+",";
    array_contents+="}";
    
    return _Fmt(".....TargetObj(%1%): data=\"%2%\", array[%1%]=%3%")
                      % cnt_          % *heapData_  % array_contents;
  }
  
  
  
  
}} // namespace lib::test
#endif
