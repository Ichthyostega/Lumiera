/*
 *  utils - collection of general purpose helpers and tools
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


/** @file utils.cpp
 ** Implementation details for some of the generic utils.
 ** 
 ** @todo WIP as of 7/21
 **
 */


#include "util/utils.hpp"
#include "util/error.hpp"

#include <regex>

using std::regex;

namespace util {

namespace {
    const regex TRUE_TOKENS { "\\s*(true|yes|on|1|\\+)\\s*",  regex::icase | regex::optimize };
    const regex FALSE_TOKENS{ "\\s*(false|no|off|0|\\-)\\s*", regex::icase | regex::optimize };

    const regex TRIMMER{"\\s*(.*?)\\s*"};
}

bool boolVal(string const& textForm)
{
    if (regex_match(textForm, TRUE_TOKENS))
        return true;
    if (regex_match(textForm, FALSE_TOKENS))
        return false;
    throw error::Invalid{"String '"+textForm+"' can not be interpreted as bool value" };
}

bool isYes (string const& textForm) noexcept
{
    return regex_match (textForm, TRUE_TOKENS);
}


string trimmed(string text)
{
    std::smatch mat;
    regex_match(text, mat, TRIMMER);
    return mat[1];
}


}//(End)namespace util
