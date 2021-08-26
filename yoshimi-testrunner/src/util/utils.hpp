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


/** @file utils.hpp
 ** Collection of helper functions and abbreviations used to simplify code.
 ** - \ref isnil(arg) checks if the argument is "empty"; argument can be a string or a container
 ** - some helper functions for working with strings (`startsWith`, `endsWith`, `removePrefix|Suffix`)
 ** - \ref trim(string) extracts the content without leading and trailing whitespace
 ** - \ref boolVal() and \ref isYes() interpret a string as boolean value
 ** - \ref contains() generic containment check for maps, strings and iterable containers
 ** - Macro \ref STRINGIFY
 ** @todo WIP as of 7/21
 **
 */



#ifndef TESTRUNNER_UTIL_UTILS_HPP_
#define TESTRUNNER_UTIL_UTILS_HPP_


#include <algorithm>
#include <string>
#include <set>

using std::string;

using uint = unsigned int;


namespace util {

/*  ======== generic empty check =========  */

/** a family of util functions providing a "no value whatsoever" test.
 *  Works on strings and all STL containers, includes NULL test for pointers
 */
template<class CONT>
inline bool isnil(const CONT &container)
{
    return container.empty();
}

template<class CONT>
inline bool isnil(const CONT *pContainer)
{
    return !pContainer or pContainer->empty();
}

template<class CONT>
inline bool isnil(CONT *pContainer)
{
    return !pContainer or pContainer->empty();
}

inline bool isnil(const char *pCStr)
{
    return !pCStr or !(*pCStr);
}

/** check if string starts with a given prefix */
inline bool startsWith(string const &str, string const &prefix)
{
    return 0 == str.rfind(prefix, 0);
}

inline bool startsWith(string const &str, const char *prefix)
{
    return 0 == str.rfind(prefix, 0);
}

/** check if string ends with the given suffix */
inline bool endsWith(string const &str, string const &suffix)
{
    size_t l = suffix.length();
    if (l > str.length())
        return false;
    size_t pos = str.length() - l;
    return pos == str.find(suffix, pos);
}

inline bool endsWith(string const &str, const char *suffix)
{
    return endsWith(str, string(suffix));
}

inline void removePrefix(string &str, string const &prefix)
{
    if (not startsWith(str, prefix))
        return;
    str = str.substr(prefix.length());
}

inline void removeSuffix(string &str, string const &suffix)
{
    if (not endsWith(str, suffix))
        return;
    str.resize(str.length() - suffix.length());
}

inline string replace(string src, string toFind, string replacement)
{
    for (size_t pos = src.find(toFind, 0);
         pos != string::npos && toFind.size();
         pos = src.find(toFind, pos+replacement.size())
        )
        src.replace(pos, toFind.size(), replacement);
    return src;
}


/** shortcut for containment test on a map */
template<typename MAP>
inline bool contains(MAP &map, typename MAP::key_type const &key)
{
    return map.find(key) != map.end();
}

/** shortcut for set value containment test */
template<typename T>
inline bool contains(std::set<T> const &set, T const &val)
{
    return set.end() != set.find(val);
}

/** shortcut for string value containment test */
template<typename T>
inline bool contains(std::string const &str, const T &val)
{
    return str.find(val) != std::string::npos;
}

/** shortcut for brute-force containment test
 *  in any sequential container */
template<typename SEQ>
inline bool contains(SEQ const &cont, typename SEQ::const_reference val)
{
    typename SEQ::const_iterator begin = cont.begin();
    typename SEQ::const_iterator end = cont.end();

    return end != std::find(begin, end, val);
}


/** @internal helper type for #backwards */
template <typename ITA>
struct ReverseIterationAdapter { ITA& iterable; };

template <typename ITA>
auto begin (ReverseIterationAdapter<ITA> adapt)
{
    return std::rbegin(adapt.iterable);
}

template <typename ITA>
auto end (ReverseIterationAdapter<ITA> adapt)
{
    return std::rend(adapt.iterable);
}

/**
 * Adapter to iterate backwards in a "foreach" loop.
 * @tparam ITA a STL compatible container with back iteration capability.
 * @remark solution based on the [Stackoverflow] from 2015 by [Prikso NAI].
 *
 * [Stackoverflow]: https://stackoverflow.com/a/28139075
 * [Prikso NAI]: https://stackoverflow.com/users/3970469/prikso-nai
 */
template <typename ITA>
inline ReverseIterationAdapter<ITA>
backwards (ITA&& iterable)
{
    return { iterable };
}


/** @return content without leading or trailing whitespace */
string trimmed(string);

/** interpret the given text as boolean value
 * @throws error::Invalid when the text is not any valid bool token
 * @remark allowed are `true false yes no on off 1 0 + -` in upper and lower case
 */
bool boolVal(string const& textForm);

/** evaluate the given text form as boolean value for `true`
 * @note other than (\ref boolVal), this function treats _everything else_ as `false`
 */
bool isYes (string const& textForm) noexcept;

} // namespace util


/** this macro wraps its parameter into a cstring literal */
#define STRINGIFY(TOKEN) __STRNGFY(TOKEN)
#define __STRNGFY(TOKEN) #TOKEN


#endif /*TESTRUNNER_UTIL_UTILS_HPP_*/
