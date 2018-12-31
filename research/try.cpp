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


/** @file try.cpp
 * Investigate the trinomial random number algorithm from the C standard library (actually GLibc 2.28).
 * Actually this is work for the yoshimi project; we try there to build an in-tree version of the PRNG,
 * in order to reduce dependencies to external libraries, which might change the sound of existing synth patches.
 */

typedef unsigned int uint;

#include "lib/format-cout.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/util.hpp"

#include <string>
#include <boost/lexical_cast.hpp>

using std::string;
using boost::lexical_cast;



#define SHOW_TYPE(_TY_) \
    cout << "typeof( " << STRINGIFY(_TY_) << " )= " << lib::meta::typeStr<_TY_>() <<endl;
#define SHOW_EXPR(_XX_) \
    cout << "Probe " << STRINGIFY(_XX_) << " ? = " << _XX_ <<endl;

namespace {
  
class StdlibPRNG
{
        char random_state[256];
        struct random_data random_buf;

    public:
        StdlibPRNG()
        {
            memset(&random_state, 0, sizeof(random_state));
        }

        bool init(uint32_t seed)
        {
            memset(random_state, 0, sizeof(random_state));
            memset(&random_buf, 0, sizeof(random_buf));
            return 0 == initstate_r(seed, random_state, sizeof(random_state), &random_buf);
        }

        uint32_t prngval()
        {
            int32_t random_result;
            random_r(&random_buf, &random_result);
            // can not fail, since &random_buf can not be NULL
            // random_result holds number 0...INT_MAX
            return random_result;
        }

        float numRandom()
        {
            return prngval() / float(INT32_MAX);
        }

        // random number in the range 0...INT_MAX
        uint32_t randomINT()
        {
            return prngval();
        }
};

class TrinomialPRNG
{
        int32_t state[63];
        int32_t *fptr;      /* Front pointer.  */
        int32_t *rptr;      /* Rear pointer.  */

    public:
        TrinomialPRNG() : fptr(NULL), rptr(NULL) { }

        bool init(uint32_t seed)
        {
            int kc = 63; /* random generation uses this trinomial: x**63 + x + 1.  */

            /* We must make sure the seed is not 0.  Take arbitrarily 1 in this case.  */
            if (seed == 0)
              seed = 1;
            state[0] = seed;

            int32_t *dst = state;
            int32_t word = seed;  // must be signed, see below
            for (int i = 1; i < kc; ++i)
            {
                /* This does:
                   state[i] = (16807 * state[i - 1]) % 2147483647;
                   but avoids overflowing 31 bits. */
                // Ichthyo 12/2018 : the above comment is only true for seed <= INT_MAX
                //                   for INT_MAX < seed <= UINT_MAX the calculation diverges from correct
                //                   modulus result, however, its values show a similar distribution pattern.
                //                   Moreover the original code used long int for 'hi' and 'lo'.
                //                   It behaves identical when using uint32_t, but not with int32_t
                uint32_t hi = word / 127773;
                uint32_t lo = word % 127773;
                word = 16807 * lo - 2836 * hi;
                if (word < 0)
                    word += 2147483647;
                *++dst = word;
            }

            fptr = &state[1];
            rptr = &state[0];
            kc *= 10;
            while (--kc >= 0)
                prngval();
            return true;
        }


        uint32_t prngval()
        {
            uint32_t val = *fptr += uint32_t(*rptr);
            uint32_t result = val >> 1;  // Chucking least random bit.
                                         // Rationale: it has a less-then optimal repetition cycle.
            int32_t *end = &state[63];
            ++fptr;
            if (fptr >= end)
              {
                fptr = state;
                ++rptr;
              }
            else
              {
                ++rptr;
                if (rptr >= end)
                  rptr = state;
              }
            // random_result holds number 0...INT_MAX
            return result;
        }


        float numRandom()
        {
            return prngval() / float(INT32_MAX);
        }

        // random number in the range 0...INT_MAX
        uint32_t randomINT()
        {
            return prngval();
        }
};

}

int
main (int, char**)
  {
    StdlibPRNG oldGen;
    TrinomialPRNG newGen;
    
    for (uint64_t seed=INT32_MAX-100; seed <= UINT32_MAX; ++seed)
      {
        oldGen.init(seed);
        newGen.init(seed);
        
        for (uint i=0; i < 5*48000; ++i)
          {
            uint32_t oval = oldGen.prngval();
            uint32_t nval = newGen.prngval();
            if (oval != nval)
              cout << "seed="<<seed << " i="<<i<< " \t "<<oval<< " != "<<nval<<endl;
          }
      }
    
    
    cout <<  "\n.gulp.\n";
    
    return 0;
  }
