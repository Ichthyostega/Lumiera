/*
  DuckDetectorExtension(Test)  -  detecting support for extension points at compile time

  Copyright (C)         Lumiera.org
    2017,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file duck-detector-extension-test.cpp
 ** unit test \ref DuckDetectorExtension_test
 */


#include "lib/test/run.hpp"
#include "lib/meta/duck-detector.hpp"
#include "lib/util.hpp"

#include <string>


namespace lib {
namespace meta{
namespace test{
  
  using std::string;
  
  
  namespace { // test types and definitions to explore....
    
    void
    fun (long)
      { }
    
    int
    fun (string, long)
      {
        return 12;
      }
    
    void
    fun ()
      { }
    
    
    class Cheesy
      { };
    
    class Fishy
      {
        /** @note type Fishy exposes an extension point `fun` */
        friend void fun (Fishy&);
      };
    
    
  }//(End) test definitions
  
  
  
  
  /***********************************************************************************//**
   * @test detect the presence of a free function extension point at compile time.
   * It is a common idiom in C++ to expose an extension point through a free function,
   * which is expected to be picked up by ADL. To mention a prominent example, any type
   * can offer the ability to be _iterated_ by injecting free functions `begin(TY)` and
   * `end(TY)`, to yield a STL compatible iterator.
   * 
   * Since such an extension point is used just by invoking the _unqualified_ function
   * with the target type, we can build a meta predicate based on the fact if such an
   * function invocation expression can be formed for the type in question.
   * 
   * @note the test really hinges on the ability to form the extension point call.
   *       For this reason, some implicit conversions might be involved, and some
   *       other conversions won't work (like passing a value to an extension point
   *       taking a reference).  
   */
  class DuckDetectorExtension_test : public Test
    {
    
      META_DETECT_EXTENSION_POINT (funZ);
      META_DETECT_EXTENSION_POINT (fun);
    
    
      void
      run (Arg) 
        {
          fun ();
          fun (23);
          fun ("FUN", 45);
          
          CHECK ( not HasExtensionPoint_funZ<long>::value );
          
          CHECK (     HasExtensionPoint_fun<long>        ::value );
          CHECK (     HasExtensionPoint_fun<long&>       ::value );
          CHECK (     HasExtensionPoint_fun<long&&>      ::value );
          CHECK (     HasExtensionPoint_fun<char>        ::value );
          CHECK (     HasExtensionPoint_fun<char&>       ::value );
          CHECK (     HasExtensionPoint_fun<char&&>      ::value );
          CHECK ( not HasExtensionPoint_fun<string>      ::value );
          CHECK ( not HasExtensionPoint_fun<void>        ::value );
          
          CHECK ( not HasExtensionPoint_fun<Cheesy>      ::value );
          CHECK ( not HasExtensionPoint_fun<Fishy>       ::value );
          CHECK (     HasExtensionPoint_fun<Fishy&>      ::value );
          CHECK ( not HasExtensionPoint_fun<Fishy&&>     ::value );
          CHECK ( not HasExtensionPoint_fun<Fishy const&>::value );
        } 
    };
  
  
  /** Register this test class... */
  LAUNCHER (DuckDetectorExtension_test, "unit meta");
  
  
  
}}} // namespace lib::meta::test
