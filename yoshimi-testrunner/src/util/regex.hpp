/*
 *  regex - helpers for working with regular expressions
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


/** @file regex.hpp
 ** Convenience wrappers and helpers for dealing with regular expressions.
 ** @see suite::step::ExeLauncher::ExeLauncher
 **
 */



#ifndef TESTRUNNER_UTIL_PARSE_HPP_
#define TESTRUNNER_UTIL_PARSE_HPP_


#include <regex>
#include <string>

namespace util {

using std::string;



/** wrapped regex iterator to allow usage in foreach loops */
struct MatchSeq
    : std::sregex_iterator
{
    MatchSeq(string const& toParse, std::regex const& regex)
        : std::sregex_iterator{toParse.begin(), toParse.end(), regex}
    { }

    using iterator = std::sregex_iterator;
    iterator begin() { return *this; }
    iterator end()   { return iterator(); }
};


}//(End)namespace util
#endif /*TESTRUNNER_UTIL_PARSE_HPP_*/
