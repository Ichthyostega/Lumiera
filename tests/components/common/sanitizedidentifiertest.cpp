/*
  SanitizedIdentifier(Test)  -  remove non-standard-chars and punctuation
 
  Copyright (C)         CinelerraCV
    2007,               Christian Thaeter <ct@pipapo.org>
 
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


#include "nobugcfg.h"

#include "common/test/run.hpp"
#include "common/util.hpp"


#include <iostream>
using std::cout;



namespace util
  {
  namespace test
    {
    
  
    class SanitizedIdentifier_test : public Test
      {
        virtual void run (Arg arg)
          {
            print_clean ("Word");
            print_clean ("a Sentence");
            print_clean ("with    a   lot  \nof Whitespace");
            print_clean ("with\"much (punctuation)!");
            print_clean ("Bääääh!!");
          }
        
        /** @test print the original and the sanitized string */
        void print_clean (const string& org)
        {
          cout << "'" << org << "' --> '" << sanitize(org) << "'\n";
        }
      };
    
      LAUNCHER (SanitizedIdentifier_test, "unit common");

      
  } // namespace test
    
} // namespace util

