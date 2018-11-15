/*
  ConfigFlags(Test)  -  generating a configuration type defined by flags

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


/** @file config-flags-test.cpp
 ** \par build a type representing a single configuration defined by a set of flags
 **
 ** The semi-automatic assembly of processing node invocation code utilises some
 ** metaprogramming to generate a factory, which in turn produces node wiring objects
 ** according to the configuration to be used for the corresponding ProcNode. This relies on
 ** generating a distinct type based on a given set of configuration flags,
 ** which is covered by this test.
 **
 ** @see configflags.hpp
 ** @see typelistmanip.hpp
 ** @see nodewiring-config.hpp real world usage example
 **
 */


#include "lib/test/run.hpp"
#include "lib/format-string.hpp"
#include "lib/meta/generator.hpp"
#include "lib/meta/typelist-manip.hpp"
#include "lib/meta/configflags.hpp"
#include "meta/typelist-diagnostics.hpp"
#include "steam/engine/nodewiring-config.hpp"
#include "lib/format-cout.hpp"
#include "lib/util.hpp"


using ::test::Test;
using util::_Fmt;
using std::string;


namespace lib  {
namespace meta {
namespace test {
  
  using proc::engine::config::Instantiation;
  
  namespace { // internal definitions
  
    enum Cases
      { ONE = 1
      , TWO
      , THR
      , FOU
      , NUM_Cases = FOU
        
      , NOT_SET   = 0
      };
    
    
    
    /* === Test data === */
    typedef Config<> Conf0;
    typedef Config<ONE> Conf1;
    typedef Config<TWO> Conf2;
    typedef Config<THR> Conf3;
    typedef Config<TWO,FOU> Conf4;
    
    typedef Flags<ONE,THR>::Tuple Flags1;
    typedef Flags<TWO,FOU>::Tuple Flags2;
    typedef Types<Flags1,Flags2> SomeFlagsets;
    
    typedef Flags<ONE,TWO,THR,FOU>::Tuple AllFlags;
    typedef CombineFlags<AllFlags>        AllFlagCombinations;
    
    
    
    /** a type which is only partially defined, for some configs.
     *  In ConfigFlags_test::check_filter() we use the metaprogramming machinery
     *  to figure out all possible configs for which \c struct Maybe is defined.
     *  (for this to work, the "defined" specialisations need to provide a
     *  typedef \c is_defined )
     */ 
    template<class CONF> struct Maybe;
    
    struct Indeed { typedef Yes_t is_defined; };
    template<> struct Maybe<Conf1> : Indeed { enum{ CODE = 10 }; };
    template<> struct Maybe<Conf3> : Indeed { enum{ CODE = 30 }; };
    
    template<uint Fl> 
    struct Maybe<Config<TWO,Fl>>
    { 
      typedef Yes_t is_defined;
      
      enum{ CODE = 20 + Fl };
    };
    
    
  } // (End) internal defs
  
#define PRINT_DELIMITER(TITLE) \
cout << "__________________________\n" \
      "__________________________ "  \
   << STRINGIFY(TITLE) << "\n";      
  
  
  
  
  
  /**********************************************************************//**
   * @test check the handling of types representing a specific configuration.
   *       Basically, this is a bitset like feature, but working on types
   *       instead of runtime values. The Goal is to detect automatically
   *       all possible defined specialisations of some template based on
   *       such configuration-tuples. This allows us to assemble the glue code
   *       for pulling data from processing nodes out of small building blocks
   *       in all possible configurations.
   */
  class ConfigFlags_test : public Test
    {
      virtual void run(Arg) 
        {
          check_testdata ();
          check_flags();
          check_instantiation ();
          check_filter ();
          check_FlagInfo ();
          check_ConfigSelector ();
        }
      
      
      void check_testdata ()
        {
          DISPLAY (Conf0);
          DISPLAY (Conf1);
          DISPLAY (Conf2);
          DISPLAY (Conf3);
          DISPLAY (Conf4);
          
          DISPLAY (AllFlags);
        }
      
      
      /** @test conversion between list-of-flags and a config-type in both directions */
      void check_flags ()
        {
          PRINT_DELIMITER (check_flags());
          
          typedef Config<TWO,FOU> Flags1;
          typedef Flags<TWO,FOU>  Flags2;
          DISPLAY (Flags1);
          DISPLAY (Flags2);
          // should denote the same type
          Flags1::Flags flags1 = Flags2::Tuple();
          Flags2::Tuple flags2 = flags1;
          CHECK (1==sizeof(flags1));   // pure marker-type without content
          CHECK (1==sizeof(flags2));
          
          typedef DefineConfigByFlags<Node<Flag<ONE>,NullType>>::Config SimpleConfig_defined_by_Typelist;
          DISPLAY (SimpleConfig_defined_by_Typelist);
          
          typedef DefineConfigByFlags<AllFlags>::Config AnotherConfig_defined_by_Typelist;
          DISPLAY (AnotherConfig_defined_by_Typelist);
        }
      
      
      /** @test creates a predicate template (metafunction) returning true
       *  iff the template \c Maybe is defined for the configuration in question
       */
      void check_instantiation ()
        {
          #define CAN_INSTANTIATE(NAME) \
          cout << "defined "             \
               << STRINGIFY(NAME)         \
               << "? ---> "                \
               << Instantiation<Maybe>::Test<NAME>::value << "\n";
          
          PRINT_DELIMITER (check_instantiation());
          
          CAN_INSTANTIATE (Conf0);
          CAN_INSTANTIATE (Conf1);
          CAN_INSTANTIATE (Conf2);
          CAN_INSTANTIATE (Conf3);
          CAN_INSTANTIATE (Conf4);
          
          typedef Config<THR,THR> Trash;
          CAN_INSTANTIATE (Trash);
        }
      
      
      /** @test given a list of flag-tuples, we first create config-types out of them
       *        and then filter out those configs for which \c template Maybe is defined
       */ 
      void check_filter ()
        {
          PRINT_DELIMITER (check_filter());
          
          DISPLAY (SomeFlagsets);
          
          typedef Apply<SomeFlagsets::List, DefineConfigByFlags> Configs_defined_by_Flagsets;
          DISPLAY (Configs_defined_by_Flagsets);
          
          typedef Filter<Configs_defined_by_Flagsets::List,Instantiation<Maybe>::Test> Filter_possible_Configs;
          DISPLAY (Filter_possible_Configs);
          
          
          DISPLAY (AllFlagCombinations);
          typedef Apply<AllFlagCombinations::List, DefineConfigByFlags> ListAllConfigs;
          DISPLAY (ListAllConfigs);
          
          typedef Filter<ListAllConfigs::List,Instantiation<Maybe>::Test> Filter_all_possible_Configs;
          DISPLAY (Filter_all_possible_Configs);
        }
      
      
      
        struct TestVisitor
          {
            string result;  ///< metafunction result
            
            TestVisitor() : result ("TestVisitor application:\n") {}
            
            /* === visitation interface === */
            typedef string Ret;
            
            Ret done()  {return result; }
            
            template<class CONF>
            void
            visit (ulong code)
              {
                result += string (_Fmt ("visit(code=%u) -->%s\n") 
                                             % code % Printer<CONF>::print() );
              }
          };
      
      /** @test FlagInfo metafunction, which takes as argument a list-of-flags
       *        as well as a list-of-lists-of-flags and especially allows to
       *        apply a visitor object to the latter 
       */
      void check_FlagInfo()
        {
          PRINT_DELIMITER (check_FlagInfo());
          
          DISPLAY (Flags1);
          cout << "max bit    : " << FlagInfo<Flags1>::BITS <<"\n";
          cout << "binary code: " << FlagInfo<Flags1>::CODE <<"\n";
          
          typedef Apply<SomeFlagsets::List, DefineConfigByFlags> SomeConfigs;
          DISPLAY (SomeConfigs);
          cout << "max bit in [SomeConfigs] : " << FlagInfo<SomeConfigs::List>::BITS <<"\n";
          
          TestVisitor visitor;
          cout << FlagInfo<SomeConfigs::List>::accept (visitor);
        }
      
      
        template<class CONF>
        struct TestFactory
          {
            uint operator() ()  { return offset_ + Maybe<CONF>::CODE; }
            TestFactory(long o) : offset_(o) {}
            
          private:
            long offset_;
          };
      
      
      /** @test use the ConfigSelector template to build a set of factories,
       *        based on a set of configurations. Then invoke the appropriate
       *        factory by specifying the configuration bit code
       */
      void check_ConfigSelector()
        {
          PRINT_DELIMITER (check_ConfigSelector());
          
          typedef Apply<AllFlagCombinations::List, DefineConfigByFlags> ListAllConfigs;
          typedef Filter<ListAllConfigs::List,Instantiation<Maybe>::Test> Possible_Configs;
          DISPLAY (Possible_Configs);
          
          using proc::engine::config::ConfigSelector;
          typedef ConfigSelector< TestFactory  // Factory template
                                , uint(void)  //  Factory function type
                                , long       //   common ctor argument
                                > TestFactorySelector;
          
          const long offset = 1000; // parameter fed to all TestFactory ctors
          TestFactorySelector testConfigSelector (Possible_Configs::List(), offset);
          
          
          #define INVOKE_CONFIG_SELECTOR(CODE) \
          cout << " Flag-code = " << CODE       \
               << " ConfigSelector() ---> "      \
               << testConfigSelector[CODE] () << "\n";
          
          INVOKE_CONFIG_SELECTOR (2);
          INVOKE_CONFIG_SELECTOR (12);
          INVOKE_CONFIG_SELECTOR (20);
          INVOKE_CONFIG_SELECTOR (4);
          INVOKE_CONFIG_SELECTOR (8);
          
          try
            {
              INVOKE_CONFIG_SELECTOR (23);
              NOTREACHED ();
            }
          catch (lumiera::error::Invalid& err)
            {
              cout << err.what() << "\n";
              lumiera_error (); // reset error flag
            }
        }
      
      
    };
  
  
  /** Register this test class... */
  LAUNCHER (ConfigFlags_test, "unit common");
  
  
  
}}} // namespace lib::meta::test
