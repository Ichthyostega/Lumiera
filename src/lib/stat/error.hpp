/*
 *  error - exceptions and error handling helpers
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


/** @file error.hpp
 ** Definition of semantic exception classes and helpers for error handling.
 ** - error::LogicBroken : violation of the application's internal logic assumptions.
 **                        Typically raising this exception implies a programming error.
 ** - error::Misconfig   : settings in configuration files or commandline miss expectations.
 ** - error::ToDo        : marker for "Stubs" or planned functionality during development.
 ** - error::State       : unexpected state or failure in system call.
 **
 ** \par Shortcuts and Helpers
 ** - Macro \ref UNIMPLEMENTED : shortcut for raising a error::ToDo
 **
 ** @todo WIP as of 7/21
 **
 */



#ifndef TESTRUNNER_UTIL_ERROR_HPP_
#define TESTRUNNER_UTIL_ERROR_HPP_


#include <stdexcept>
#include <string>

using std::string;



namespace error {

using std::logic_error;


class LogicBroken : public logic_error
{
public:
    LogicBroken(string msg)
        : logic_error{"LogicBroken: " + msg}
    { }
};


class Misconfig : public logic_error
{
public:
    Misconfig(string msg)
        : logic_error{"Misconfig: "+msg}
    { }
};


class Invalid : public logic_error
{
public:
    Invalid(string msg)
        : logic_error{"Invalid Data: "+msg}
    { }
};


class State : public logic_error
{
public:
    State(string msg)
        : logic_error{"Unforeseen state: "+msg}
    { }
};


class FailedLaunch : public State
{
public:
    FailedLaunch(string msg)
        : State{"Launch of Test Case failed -- "+msg}
    { }
};


class ToDo : public logic_error
{
public:
    ToDo(string msg) :
        logic_error{"UNIMPLEMENTED: "+msg}
    { }
};


} // namespace error


#define UNIMPLEMENTED(_MSG_) \
    throw error::ToDo(_MSG_)

#endif /*TESTRUNNER_UTIL_ERROR_HPP_*/
