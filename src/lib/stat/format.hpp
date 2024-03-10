/*
 *  format - collection of test formatting helpers
 *
 *  Copyright 2021, Hermann Vosseler <Ichthyostega@web.de>
 *
 *  This file is part of the Yoshimi-Testsuite, which is free software:
 *  you can redistribute and/or modify it under the terms of the GNU
 *  General Public License as published by the Free Software Foundation,
 *  either version 3 of the License, or (at your option) any later version.
 *
 *  Yoshimi-Testsuite is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with yoshimi.  If not, see <http://www.gnu.org/licenses/>.
 ***************************************************************/


/** @file format.hpp
 ** Collection of helper functions for text and number output and formatting.
 ** @todo WIP as of 7/21
 **
 */



#ifndef TESTRUNNER_UTIL_FORMAT_HPP_
#define TESTRUNNER_UTIL_FORMAT_HPP_


#include "util/utils.hpp"

#include <string>
#include <sstream>

using std::string;



namespace util
{


/** format number as string */
template<typename NUM>
inline string str(NUM n)
{
   std::ostringstream oss;
   oss << n;
   return oss.str();
}


template<typename X>
inline string formatVal(X x)
{
    return str(x);
}

inline string formatVal(string s)
{
    return "\""+s+"\"";
}

inline string formatVal(bool yes)
{
    return yes? "true":"false";
}

inline string formatVal(float f)
{
   std::ostringstream oss;
   oss.precision(3);
   oss.width(5);
   oss << f;
   return oss.str();
}


/** parse string representation into typed value */
template<typename TAR>
inline TAR parseAs(string const& encodedVal)
{
    std::istringstream converter{encodedVal};
    TAR value;
    converter >> value;
    if (converter.fail())
        throw error::Invalid("unable to parse "+formatVal(encodedVal));
    return value;
}

template<>
inline bool parseAs(string const& encodedBool)
{
    return util::boolVal(encodedBool);
}
template<>
inline string parseAs(string const& string)
{
    return string; // pass-through (even if empty)
}



}//namespace util
#endif /*TESTRUNNER_UTIL_FORMAT_HPP_*/
