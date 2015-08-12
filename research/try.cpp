/* try.cpp  -  for trying out some language features....
 *             scons will create the binary bin/try
 *
 */

// 8/07  - how to control NOBUG??
//         execute with   NOBUG_LOG='ttt:TRACE' bin/try
// 1/08  - working out a static initialisation problem for Visitor (Tag creation)
// 1/08  - check 64bit longs
// 4/08  - comparison operators on shared_ptr<Asset>
// 4/08  - conversions on the value_type used for boost::any
// 5/08  - how to guard a downcasting access, so it is compiled in only if the involved types are convertible
// 7/08  - combining partial specialisation and subclasses 
// 10/8  - abusing the STL containers to hold noncopyable values
// 6/09  - investigating how to build a mixin template providing an operator bool()
// 12/9  - tracking down a strange "warning: type qualifiers ignored on function return type"
// 1/10  - can we determine at compile time the presence of a certain function (for duck-typing)?
// 4/10  - pretty printing STL containers with python enabled GDB?
// 1/11  - exploring numeric limits
// 1/11  - integer floor and wrap operation(s)
// 1/11  - how to fetch the path of the own executable -- at least under Linux?
// 10/11 - simple demo using a pointer and a struct
// 11/11 - using the boost random number generator(s)
// 12/11 - how to detect if string conversion is possible?
// 1/12  - is partial application of member functions possible?
// 5/14  - c++11 transition: detect empty function object
// 7/14  - c++11 transition: std hash function vs. boost hash
// 9/14  - variadic templates and perfect forwarding
// 11/14 - pointer to member functions and name mangling
// 8/15  - Segfault when loading into GDB (on Debian/Jessie 64bit


/** @file try.cpp
 ** Investigation: Segfault when loading into GDB (on Debian/Jessie 64bit)
 **
 */

#include "lib/test/test-helper.hpp"
#include "lib/util.hpp"
#include "lib/format-util.hpp"
#include "lib/diff/record.hpp"
#include "lib/itertools.hpp"
#include "lib/util.hpp"       //////TODO necessary?

#include <iostream>
//#include <utility>
#include <string>
#include <vector>

using std::string;
using util::isSameObject;
using util::isnil;
using std::vector;
//using std::swap;
using std::cout;
using std::endl;


namespace lib {
namespace diff{
namespace test{
  
//  using lumiera::error::LUMIERA_ERROR_LOGIC;
  using lumiera::error::LUMIERA_ERROR_INVALID;
  using lumiera::error::LUMIERA_ERROR_BOTTOM_VALUE;
  
  namespace {//Test fixture....
    
    using Seq  = vector<string>;
    using RecS = Record<string>;
    
    template<class IT>
    inline Seq
    contents (IT const& it)
    {
      Seq collected;
      append_all (it, collected);
      return collected;
    }
    
    inline Seq
    contents (RecS const& rec_of_strings)
    {
      return contents (rec_of_strings.begin());
    }
    
    template<class X>
    inline Seq
    strings (std::initializer_list<X> const& con)
    {
      Seq collected;
      for (auto elm : con)
        collected.push_back(elm);
      return collected;
    }
    
    
  }//(End)Test fixture
  
  
  
  
  
  
  
  /*************************************************************************************//**
   * @test Verify properties of a special collection type meant for external representation
   *       of object-like data.
   *       
   * @see IndexTable
   * @see DiffListApplication_test
   */
  class GenericRecordRepresentation_test//  : public Test
    {
    public:
      virtual void
      run ()
        {
          simpleUsage();
        }
      
      
      void
      simpleUsage()
        {
          RecS enterprise("starship"
                         , strings ({"Name = USS Enterprise"
                                    ,"Registry = NCC-1701-D"
                                    ,"Class = Galaxy"
                                    ,"Owner = United Federation of Planets"
                                    ,"built=2363"
                                   })
                         , strings ({"Picard", "Riker", "Data", "Troi", "Worf", "Crusher", "La Forge"})
                         );
          
          cout << "enterprise = " << string(enterprise)<<endl;
        }
    };

}}}



int
main (int, char**)
  {
    lib::diff::test::GenericRecordRepresentation_test mist;
    mist.run();
    
    cout <<  "\n.gulp.\n";
    
    return 0;
  }
