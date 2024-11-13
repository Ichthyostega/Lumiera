/*
  FormatString(Test)  -  validate string template formatting (printf style)

  Copyright (C)         Lumiera.org
    2011,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file format-string-test.cpp
 ** unit test \ref FormatString_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/format-string.hpp"
#include "lib/symbol.hpp"
#include "lib/error.hpp"
#include "lib/util.hpp"

#include <boost/lexical_cast.hpp>
#include <iostream>

using boost::lexical_cast;
using std::cout;
using std::endl;


namespace util {
namespace test {
  
  namespace error = lumiera::error;
  
  
  
  /***************************************************************************//**
   * @test verifies our front-end for printf-style format string based formatting.
   *       - the implementation is based on boost::format
   *       - we create a wrapper-object on-the fly, which is able to hold
   *         the result of a partial invocation, until all parameters are given.
   *       - explicit specialisations feed all primitive types directly down
   *         into the implementation level. If possible, a custom operator string()
   *         will be used for formatting.
   *       - exceptions will be logged, but handled gracefully
   * 
   * @see format-string.hpp
   * @see boost::format
   */
  class FormatString_test : public Test
    {
      void
      run (Arg)
        {
          check_simpleInvocation();
          check_delayedInvocation();
          verify_typeHandling();
          verify_customType();
          verify_errorHandling();
          verify_pointerHandling();
        }
      
      
      
      void
      check_simpleInvocation ()
        {
          string formatted = _Fmt("--format-template--int=%04d--double=%+5.2f--string=%-9s--")
                                 % 12
                                 % 1.228
                                 % "Lumiera";
          cout << formatted << endl;
          CHECK (formatted == "--format-template--int=0012--double=+1.23--string=Lumiera  --");
        }
      
      
      void
      check_delayedInvocation ()
        {
          string formatString = "%p %|20T_| %u";
          _Fmt formatter (formatString);
          
          uint val = rani (100);
          void *pt = &val;
          
          formatter % pt;
          formatter % val;
          
          cout << formatter << endl;
        }
      
      
      void
      verify_typeHandling ()
        {
          int i(-12);
          CHECK (_Fmt("%d") % i     == "-12"    );
          CHECK (_Fmt("%6d") % i    == "   -12" );
          CHECK (_Fmt("%-6d") % i   == "-12   " );
          CHECK (_Fmt("%+-6d") % -i == "+12   " );
          CHECK (_Fmt("%+06d") % -i == "+00012" );
          CHECK (_Fmt("%06X") % -i  == "00000C" );
          CHECK (_Fmt("%#x") % -i   == "0xc"    );
          
          uint u(12);
          CHECK (_Fmt("%d") % u     == "12"     );
          CHECK (_Fmt("%6d") % u    == "    12" );
          CHECK (_Fmt("%-6d") % u   == "12    " );
          CHECK (_Fmt("%-+6d") % u  == "12    " );
          CHECK (_Fmt("%+06d") % u  == "000012" );
          
          short sh(-123);
          CHECK (_Fmt("%6d") % sh   == "  -123" );
          
          ushort ush(123);
          CHECK (_Fmt("%6d") % ush  == "   123" );
          
          long l(-123);
          CHECK (_Fmt("%6d") % l    == "  -123" );
          
          ulong ul(123);
          CHECK (_Fmt("%6d") % ul   == "   123" );
          
          int64_t ll(5e+9);
          CHECK (_Fmt("%d") % ll    == "5000000000" );
          CHECK (_Fmt("%5.3d") %ll  == "5000000000" );
          CHECK (_Fmt("%10.3e") %ll == "5000000000" );
          
          uint64_t ull(ll);
          CHECK (_Fmt("%d") % ull   == "5000000000" );
          
          float f(12.34);
          CHECK (_Fmt("%g")   % f   == "12.34"  );
          CHECK (_Fmt("%d")   % f   == "12.34"  );
          CHECK (_Fmt("%.3f") % f   == "12.340" );
          CHECK (_Fmt("%.1e") % f   == "1.2e+01");
          CHECK (_Fmt("%.0f") % f   == "12"     );
          
          double d(-12.34);
          CHECK (_Fmt("%g") % d     == "-12.34" );
          
          char c(0x40);
          CHECK (_Fmt("%d") % c        == "@"   );
          CHECK (_Fmt("%x") % c        == "@"   );
          CHECK (_Fmt("%o") % c        == "@"   );
          CHECK (_Fmt("%c") % c        == "@"   );
          CHECK (_Fmt("%s") % c        == "@"   );
          CHECK (_Fmt("%d") % short(c) == "64"  );
          CHECK (_Fmt("%x") % short(c) == "40"  );
          CHECK (_Fmt("%o") % short(c) == "100" );
          CHECK (_Fmt("%c") % short(c) == "6"   );
          CHECK (_Fmt("%s") % short(c) == "64"  );
          
          uchar uc(0xff);
          CHECK (_Fmt("%d") % uint(uc) == "255" );
          CHECK (_Fmt("%x") % uint(uc) == "ff"  );
          CHECK (_Fmt("%X") % uint(uc) == "FF"  );
          CHECK (_Fmt("%c") % uint(uc) == "2"   );
          CHECK (_Fmt("%s") % uint(uc) == "255" );
          CHECK (_Fmt("%d") % uc       == "\xFF");
          CHECK (_Fmt("%x") % uc       == "\xFF");
          CHECK (_Fmt("%X") % uc       == "\xFF");
          CHECK (_Fmt("%c") % uc       == "\xFF");
          CHECK (_Fmt("%s") % uc       == "\xFF");
          
          c = char(uc);
          CHECK (_Fmt("%c") % c        == "\xFF");
          
          string str("Lumiera");
          CHECK (_Fmt("%s") % str   == "Lumiera"    );
          CHECK (_Fmt("%10s") % str == "   Lumiera" );
          CHECK (_Fmt("%7.4s") %str == "   Lumi"    );
          CHECK (_Fmt("%10c") % str == "         L" );
          
          const char* pch("edit");
          CHECK (_Fmt("%s") %   pch == "edit"       );
          CHECK (_Fmt("%10s") % pch == "      edit" );
          CHECK (_Fmt("%7.3s") %pch == "    edi"    );
          CHECK (_Fmt("%10c") % pch == "         e" );
          
          lib::Literal lit("your");
          CHECK (_Fmt("%s") %   lit == "your"       );
          CHECK (_Fmt("%10s") % lit == "      your" );
          CHECK (_Fmt("%7.2s") %lit == "     yo"    );
          CHECK (_Fmt("%10c") % lit == "         y" );
          
          lib::Symbol sym("freedom");
          CHECK (_Fmt("%s") %   sym == "freedom"    );
          CHECK (_Fmt("%10s") % sym == "   freedom" );
          CHECK (_Fmt("%7.5s") %sym == "  freed"    );
          CHECK (_Fmt("%10c") % sym == "         f" );
        }
      
      
      /* == some custom types to test with == */
      
      struct Silent
        {
          int i_;
          
          Silent(int i) : i_(i) { }
        };
      
      struct Verbose
        : Silent
        {
          using Silent::Silent;
          virtual ~Verbose() { }
          
          virtual
          operator string()  const
            {
              return _Fmt("Number-%03d") % i_; 
            }
        };
      
      struct Explosive
        : Verbose
        {
          using Verbose::Verbose;
          
          operator string()  const
            {
              if (23 == i_)
                throw error::State("encountered Fantomas");
              else
                return Verbose::operator string();
            }
        };
      
      
      void
      verify_customType ()
        {
          Silent s(12);
          Verbose v(13);
          Explosive x(23);
          
          Silent& rs1 = v;
          Silent& rs2 = x;
          
          Verbose& rv = x;
          
          string typeDisplay = _Fmt("%s") % s;
          cout << typeDisplay << endl;
          CHECK (contains (typeDisplay, "test"));
          CHECK (contains (typeDisplay, "FormatString_test"));
          CHECK (contains (typeDisplay, "Silent"));
          
          CHECK (_Fmt("!!%s!!") % v  == "!!Number-013!!");
          CHECK (_Fmt("!!%s!!") % x  == "!!<string conversion failed: LUMIERA_ERROR_STATE:unforeseen state (encountered Fantomas).>!!");
          
          CHECK (contains (_Fmt("%s") % rs1, "Silent"));
          CHECK (contains (_Fmt("%s") % rs2, "Silent"));
          
          CHECK (_Fmt("!!%s!!") % rv  == "!!<string conversion failed: LUMIERA_ERROR_STATE:unforeseen state (encountered Fantomas).>!!");
          
          x.i_ = 42;
          CHECK (_Fmt("!!%s!!") % rv  == "!!Number-042!!");
          CHECK (_Fmt("!!%s!!") % x   == "!!Number-042!!");
          
          lib::Symbol sym("42");
          CHECK (_Fmt("!!%s!!") % sym == "!!42!!");   // but especially Symbol datatype is explicitly treated like a string
        }
      
      
      void
      verify_errorHandling ()
        {
          cout << _Fmt("__nix_") % 1 % 2 << endl;
          cout << _Fmt("__%d__")         << endl;
          cout << _Fmt("__%d__") % 1     << endl;
          cout << _Fmt("__%d__") % 1 % 2 << endl;
          
          const char* evil = NULL;
          cout << _Fmt("__%d__") %  evil  << endl;
          cout << _Fmt("__%d__") % "dirt" << endl;
          cout << _Fmt("__%d__") % "1234" << endl;
          cout << _Fmt("__%d__") % "0xff" << endl;
          
          VERIFY_ERROR(FORMAT_SYNTAX, _Fmt("%madness"));
        }
      
      
      void
      verify_pointerHandling ()
        {
          int             i(-12);     int       *   pi = &  i;
          uint             u(12);     uint      *   pu = &  u;
          short         sh(-123);     short     *  psh = & sh;
          ushort        ush(123);     ushort    * push = &ush;
          long           l(-123);     long      *   pl = &  l;
          ulong          ul(123);     ulong     *  pul = & ul;
          int64_t       ll(5e+9);     int64_t   *  pll = & ll;
          uint64_t       ull(ll);     uint64_t  * pull = &ull;
          float         f(12.34);     float     *   pf = &  f;
          double       d(-12.34);     double    *   pd = &  d;
          uchar         uc(0xff);     uchar     *  puc = & uc;
          string  str("Lumiera");     string    * pstr = &str;
          
          CHECK (_Fmt("%d") %   i == _Fmt("%d") %   pi);
          CHECK (_Fmt("%d") %   u == _Fmt("%d") %   pu);
          CHECK (_Fmt("%d") %  sh == _Fmt("%d") %  psh);
          CHECK (_Fmt("%d") % ush == _Fmt("%d") % push);
          CHECK (_Fmt("%d") %   l == _Fmt("%d") %   pl);
          CHECK (_Fmt("%d") %  ul == _Fmt("%d") %  pul);
          CHECK (_Fmt("%d") %  ll == _Fmt("%d") %  pll);
          CHECK (_Fmt("%d") % ull == _Fmt("%d") % pull);
          CHECK (_Fmt("%e") %   f == _Fmt("%e") %   pf);
          CHECK (_Fmt("%e") %   d == _Fmt("%e") %   pd);
          CHECK (_Fmt("%g") %  uc == _Fmt("%g") %  puc);
          CHECK (_Fmt("%s") % str == _Fmt("%s") % pstr);
          
          char c('a');
          char pc[2] = {'a', '\0'};
          CHECK (_Fmt("%g") %   c == _Fmt("%g") %   pc);
          
          Verbose v(42);
          Verbose *pv = &v;
          void    *vv = pv;
          CHECK (_Fmt("__%s__") % v  == "__Number-042__");
          CHECK (_Fmt("__%s__") % pv == "__Number-042__");
          CHECK (_Fmt("__%s__") % vv != "__Number-042__");
          
          string address = lexical_cast<string>(pv);
          CHECK (_Fmt("__%s__") % vv == "__"+address+"__");
          
          pv = NULL;
          vv = NULL;
          CHECK (_Fmt("__%s__") % pv == "__"+BOTTOM_INDICATOR+"__");
          CHECK (_Fmt("__%s__") % vv == "__0__");
        }
    };
  
  LAUNCHER (FormatString_test, "unit meta");
  
  
}} // namespace util::test

