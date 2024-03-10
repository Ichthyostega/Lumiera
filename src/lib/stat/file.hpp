/*
 *  file - filesystem access and helpers
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


/** @file file.hpp
 ** Includes the C++ Filesystem library and provides some convenience helpers.
 ** The `std::filesystem` library allows for portable access to file and directory handling
 ** functions; this header maps these functions with a convenient `fs::` namespace prefix,
 ** and offers some convenience extensions, which are _"slightly non-portable"_ (they were
 ** developed on Linux and "should" work on Unix like systems; adapters for exotic operating
 ** systems could be added here when necessary...)
 **
 */



#ifndef TESTRUNNER_UTIL_FILE_HPP_
#define TESTRUNNER_UTIL_FILE_HPP_


#include "util/error.hpp"

#include <filesystem>
#include <cstdlib>


namespace fs = std::filesystem;
namespace std::filesystem {

const string       UNIX_HOMEDIR_SYMBOL = "~";
const char * const UNIX_HOMEDIR_ENV    = "HOME";


inline fs::path getHomePath()
{
    auto home = std::getenv(UNIX_HOMEDIR_ENV);
    if (not home)
        throw error::Misconfig("Program environment doesn't define $HOME (Unix home directory).");
    return fs::path{home};
}


/** resolves symlinks, `~` (Unix home dir) and relative specs
 * @return absolute canonical form if the path exists;
 *         otherwise only the home directory is expanded */
inline fs::path consolidated(fs::path rawPath)
{
    if (rawPath.empty())
        return rawPath;
    if (UNIX_HOMEDIR_SYMBOL == *rawPath.begin())
        rawPath = getHomePath() / rawPath.lexically_proximate(UNIX_HOMEDIR_SYMBOL);

    return fs::exists(rawPath)? fs::absolute(
                                fs::canonical(rawPath))
                              : rawPath;
}

}//(End)namespace fs

namespace util {

inline string formatVal(fs::path path)
{
    return "\""+string{path}+"\"";
}

}//(End)namespace util
#endif /*TESTRUNNER_UTIL_TEE_HPP_*/
