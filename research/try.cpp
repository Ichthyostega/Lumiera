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
// 8/15  - generalising the Variant::Visitor
// 1/16  - generic to-string conversion for ostream
// 1/16  - build tuple from runtime-typed variant container
// 3/17  - generic function signature traits, including support for Lambdas
// 9/17  - manipulate variadic templates to treat varargs in several chunks
// 11/17 - metaprogramming to detect the presence of extension points
// 11/17 - detect generic lambda
// 12/17 - investigate SFINAE failure. Reason was indirect use while in template instantiation
// 03/18 - Dependency Injection / Singleton initialisation / double checked locking
// 04/18 - investigate construction of static template members
// 08/18 - Segfault when compiling some regular expressions for EventLog search
// 10/18 - investigate insidious reinterpret cast
// 12/18 - investigate the trinomial random number algorithm from the C standard lib
// 04/19 - forwarding tuple element(s) to function invocation
// 06/19 - use a statefull counting filter in a treeExplorer pipeline
// 03/20 - investigate type deduction bug with PtrDerefIter
// 01/21 - look for ways to detect the presence of an (possibly inherited) getID() function
// 08/22 - techniques to supply additional feature selectors to a constructor call
// 10/23 - search for ways to detect signatures of member functions and functors uniformly
// 11/23 - prototype for a builder-DSL to configure a functor to draw and map random values
// 11/23 - prototype for grouping from iterator


/** @file try.cpp
 * Investigate how best to integrate a grouping device into the iterator pipeline framework.
 */

typedef unsigned int uint;


#include "lib/format-cout.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/test/diagnostic-output.hpp"
#include "lib/format-util.hpp"
#include "lib/util.hpp"

#include "lib/iter-explorer.hpp"
#include "lib/test/test-coll.hpp"
#include <utility>
#include <array>

using std::forward;
using std::move;

namespace lib {
  namespace iter_explorer {
    
    template<class SRC, class RES, uint grp>
    class Groupy
      : public SRC
      {
        static_assert(can_IterForEach<SRC>::value, "Lumiera Iterator required as source");
        
      protected:
        using Group = std::array<RES, grp>;
        using Iter  = typename Group::iterator;
        union Buffer
          {
            char storage[sizeof(Group)];
            Group group;
            
            Iter begin() { return group.begin();}
            Iter end()   { return group.end();  }
          };
        Buffer buff_;
        uint   pos_{0};
        
        
      public:
        using value_type = Group;
        using reference  = Group&;
        using pointer    = Group*;

        Groupy() =default;
        // inherited default copy operations
        
        Groupy (SRC&& dataSrc)
          : SRC{move (dataSrc)}
          {
            pullGroup(); // initially pull to establish the invariant
          }
        
        
        /**
         * Iterate over the Elements in the current group.
         * @return a Lumiera Forward Iterator with value type RES
         */
        auto
        getGroupedElms()
          {
            ENSURE (buff_.begin()+pos_ <= buff_.end());
                   // Array iterators are actually pointers 
            return RangeIter{buff_.begin(), buff_.begin()+pos_};
          }
        
        /**
         * Retrieve the tail elements produced by the source,
         * which did not suffice to fill a full group.
         * @remark getRest() is NIL during regular iteration, but
         *         possibly yields elements when checkPoint() = false;
         */
        auto
        getRestElms()
          {
            return checkPoint()? RangeIter<Iter>()
                               : getGroupedElms();
          }
        
        /** refresh state when other layers manipulate the source sequence.
         * @note possibly pulls to re-establish the invariant */
        void
        expandChildren()
          {
            SRC::expandChildren();
            pullGroup();
          }
        
      public: /* === Iteration control API for IterableDecorator === */
        
        bool
        checkPoint()  const
          {
            return pos_ == grp;
          }
        
        reference
        yield()  const
          {
            return unConst(buff_).group;
          }
        
        void
        iterNext()
          {
            pullGroup();
          }
        
        
      protected:
        SRC&
        srcIter()  const
          {
            return unConst(*this);
          }
        
        /** @note establishes the invariant:
         *        source has been consumed to fill a group */
        void
        pullGroup ()
          {
            for (pos_=0
                ; pos_<grp and srcIter()
                ; ++pos_,++srcIter()
                )
              buff_.group[pos_] = *srcIter();
          }
      };
    
      template<uint grp, class IT>
      auto
      groupy (IT&& src)
        {
          using Value   = typename meta::ValueTypeBinding<IT>::value_type;
          using ResCore = Groupy<IT, Value, grp>;
          using ResIter = typename _DecoratorTraits<ResCore>::SrcIter;
          
          return IterExplorer<ResIter> (ResCore {move(src)});
        }
    
  }//iter_explorer
}//lib

using lib::test::getTestSeq_int;
using lib::test::VecI;

    /** Diagnostic helper: join all the elements from a _copy_ of the iterator */
    template<class II>
    inline string
    materialise (II&& ii)
    {
      return util::join (std::forward<II> (ii), "-");
    }

template<uint num, uint grp>
void
test()
  {
    VecI vec1 = getTestSeq_int<VecI> (num);
    cout <<"---"<<grp<<" of "<<num<<"---\n"
         << materialise(vec1) <<endl;
    
    auto it = lib::explore(vec1);
    auto groupie = lib::explore(lib::iter_explorer::groupy<grp> (move(it)))
                       .transform([](auto it){ return "["+util::join(*it)+"]"; });

    for ( ;groupie; ++groupie)
      cout << *groupie<<"-";

    CHECK (not groupie);
    CHECK (groupie.getGroupedElms());
    CHECK (groupie.getRestElms());
    for (auto r = groupie.getRestElms() ;r; ++r)
      cout << *r<<"+";
    cout << endl;
  }

int
main (int, char**)
  {
    test<10,3>();
    test<13,5>();
    test<55,23>();
    test<23,55>();
    
    cout <<  "\n.gulp.\n";
    return 0;
  }
