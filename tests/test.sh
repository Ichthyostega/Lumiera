#!/usr/bin/env bash
#
#  Copyright (C)
#    2007-2010,       Christian Thaeter <ct@pipapo.org>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of the
# License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

# TESTMODE=FULL yet unimplemented
#   run all tests, PLANNED which fail count as error
#
# TESTMODE=FAST
#   run only tests which recently failed
#
# TESTMODE=FIRSTFAIL
#   stop testing on the first failure

#intro Test.sh
#intro =======
#intro Christian Thäter
#intro
#intro A shell script driving software tests.
#intro

#=intro
#=tests Writing tests
# =run
#=config
#=configf
# =make
# =control
#=valgrind
#=libtool
#_
#_ Index
#_ -----
#_
#=index
#_

LC_ALL=C

#config HEAD- Configuration; configuration; configure tests
#config
#config PARA LOGSUPPRESS; LOGSUPPRESS; suppress certain lines from stderr
#config  LOGSUPPRESS='^\(\*\*[0-9]*\*\* \)\?[0-9]\{10,\}: \(TRACE\|INFO\|NOTICE\|WARNING\|ERR\):'
#config
#config Programms sometimes emit additional diagnostics on stderr which is volatile and not necessary for
#config validating the output the `LOGSUPRESS` variable can be set to a regex to filter this things out.
#config The default as shown above filters some NoBug annotations and non fatal logging out.
#config
LOGSUPPRESS='^\(\*\*[0-9]*\*\* \)\?[0-9]\{10,\}[:!] \(TRACE\|INFO\|NOTICE\|WARNING\|ERR\):'



#configf HEAD~ Configuration Files; configuration files; define variables to configure the test
#configf
#configf `test.sh` reads config files from the following location if they are exist
#configf * 'test.conf' from the current directory
#configf * '$srcdir/test.conf' `$srcdir` is set by autotools
#configf * '$srcdir/tests/test.conf' `tests/` is suspected as default directory for tests
#configf * '$TEST_CONF' a user defineable variable to point to a config file
#configf
test -f 'test.conf' && source test.conf
test -n "$srcdir" -a -e "$srcdir/test.conf" && source "$srcdir/test.conf"
test -n "$srcdir" -a -e "$srcdir/tests/test.conf" && source "$srcdir/tests/test.conf"
test -n "$TEST_CONF" -a -e "$TEST_CONF" && source "$TEST_CONF"



#config PARA Resource Limits; ulimit; constrain resource limits
#config It is possible to set some limits for tests to protect the system against really broken cases.
#config Since running under valgrind takes consinderable more resources there are separate variants for
#config limits when running under valgrind.
#config
#config  LIMIT_CPU=5
#config Maximal CPU time in seconds the test may take after it will be killed with SIGXCPU. This protects agaist Lifelocks.
#config
#config  LIMIT_TIME=10
#config Maximal wall-time a test may take after this it will be killed with SIGKILL. Protects against Deadlocks.
#config
#config  LIMIT_VSZ=$((512*1024))
#config Maximal virtual memory size the process may map, allocations/mappings will fail when this limit is reached.
#config Protects against memory leaks. The value is given in KiB.
#config
#config  LIMIT_VG_CPU=20
#config  LIMIT_VG_TIME=30
#config  LIMIT_VG_VSZ=$LIMIT_VSZ
#config Same variables again with limits when running under valgrind.
#config
LIMIT_CPU="${LIMIT_CPU:-5}"
LIMIT_VSZ="${LIMIT_VSZ:-$((512*1024))}"
LIMIT_TIME="${LIMIT_TIME:-$((2 * ${LIMIT_CPU}))}"

LIMIT_VG_VSZ="${LIMIT_VG_VSZ:-$LIMIT_VSZ}"
LIMIT_VG_CPU="${LIMIT_VG_CPU:-$((4 * LIMIT_CPU))}"
LIMIT_VG_TIME="${LIMIT_VG_TIME:-$((3 * ${LIMIT_TIME}))}"



arg0="$0"
TESTDIR="$(dirname "$arg0")"


#libtool HEAD Libtool; libtool; support for libtool
#libtool When test.sh detects the presence of './libtool' it runs all tests with
#libtool `./libtool --mode=execute`.
#libtool
LIBTOOL_EX=
if test -x ./libtool; then
    LIBTOOL_EX="./libtool --mode=execute"
fi

#valgrind HEAD- Valgrind; valgrind; valgrind support
#valgrind Test are run under valgrind supervision by default, if not disabled.
#valgrind
#valgrind PARA VALGRINDFLAGS; VALGRINDFLAGS; control valgrind options
#valgrind  VALGRINDFLAGS="--leak-check=yes --show-reachable=yes"
#valgrind
#valgrind `VALGRINDFLAGS` define the options which are passed to valgrind. This can be used to override
#valgrind the defaults or switching the valgrind tool. The special case `VALGRINDFLAGS=DISABLE` will disable
#valgrind valgrind for the tests.
#valgrind
#valgrind HEAD~ Generating Valgrind Suppression Files; vgsuppression; ignore false positives
#valgrind When there is a 'vgsuppression' executable in the current dir (build by something external) then
#valgrind test.sh uses this to generate a local 'vgsuppression.supp' file and uses that to suppress
#valgrind all errors generated by 'vgsuppression'. The Idea here is that one adds code which triggers known
#valgrind false positives in 'vgsuppression'. Care must be taken that this file is simple and does
#valgrind not generate true positives.
#valgrind
valgrind=""
if [ "$VALGRINDFLAGS" = 'DISABLE' ]; then
    echo "valgrind explicitly disabled"
else
    if [ "$(which valgrind)" ]; then
    LIMIT_CPU="$LIMIT_VG_CPU"
    LIMIT_VSZ="$LIMIT_VG_VSZ"
    LIMIT_TIME="$LIMIT_VG_TIME"
        if [[ -x 'vgsuppression' ]]; then
            if [[ 'vgsuppression' -nt 'vgsuppression.supp' ]]; then
                echo 'generating valgrind supression file'

                $LIBTOOL_EX $(which valgrind) ${VALGRINDFLAGS:---leak-check=yes --show-reachable=yes} -q --gen-suppressions=all vgsuppression 2>&1 \
                    | awk '/^{/ {i = 1;} /^}/ {i = 0; print $0;} {if (i == 1) print $0;}' >vgsuppression.supp
            fi
            valgrind="$(which valgrind) ${VALGRINDFLAGS:---leak-check=yes --show-reachable=no} --suppressions=vgsuppression.supp -q"
        else
            valgrind="$(which valgrind) ${VALGRINDFLAGS:---leak-check=yes --show-reachable=no -q}"
        fi
    else
        echo "no valgrind found, go without it"
    fi
fi
# set up the final resource limits...
echo "NOTE: CPU  time limit: $LIMIT_CPU sec"
echo "    : real time limit: $LIMIT_TIME sec"
echo "    : VMem size limit: $(( $LIMIT_VSZ / 1024)) MiB"
ulimit -S -t ${LIMIT_CPU:-5} -v ${LIMIT_VSZ:-524288}


echo
echo "================ ${0##*/} ================"

TESTCNT=0
SKIPCNT=0
FAILCNT=0


# the old testlog if existing will be used to check for previous test states
if test -f ,testlog; then
    mv ,testlog ,testlog.pre
else
    touch ,testlog.pre
fi

date >,testlog

function compare_template() # template plainfile
{
    local template
    local line
    local miss
    local lineno=1
    local templateno=1
    {
        IFS='' read -u 3 -r template || return 0
        IFS='' read -u 4 -r line || { echo "no output"; return 1; }
        while true; do
            local cmd="${template%%:*}:"
            local arg="${template#*: }"

            case $cmd in
            'regex_cont:')
                if [[ $line =~ $arg ]]; then
                    IFS='' read -u 4 -r line ||
                    if IFS='' read -u 3 -r template; then
                        echo "premature end in output, expecting $template:$templateno"
                        return 1
                    else
                        return 0
                    fi
                    : $((++lineno))
                    miss=0
                else
                    if [[ $((++miss)) -gt 1 ]]; then
                        echo -e "'$line':$lineno\ndoes not match\n$template:$templateno"
                        return 1
                    fi
                    IFS='' read -u 3 -r template || { echo "more output than expected: '$line':$lineno"; return 1; }
                    : $((++templateno))
                fi
                ;;
            'literal:')
                if [[ "$line" = "$arg" ]]; then
                    IFS='' read -u 3 -r template && IFS='' read -u 4 -r line || {
                        return 0
                    }
                else
                    echo -e "'$line':$lineno\ndoes not match\n$template:$templateno"
                    return 1
                fi
                ;;
            *)
                echo "UNKOWN MATCH COMMAND '$cmd'" 1>&2
                exit
                ;;
            esac
        done
    } 3<"$1" 4<"$2"
}

#tests HEAD- Writing Tests; tests; how to write testsuites
#tests Tests are nothing more than bash scripts with some functions from the test.sh
#tests framework defined. Test.sh looks in the current directory for all files which ending in .test
#tests and runs them in alphabetical order. The selection of this tests can be constrained with the
#tests `TESTSUITES` environment variable.
#tests
#tests HEAD~ Testsuites; test files; writing tests
#tests It is common to start the name of the '.test' files with a 2 digit number to give them a proper
#tests order: '10foo.test', '20bar.test' and so on. Each such test should only test a certain aspect of
#tests the system. You have to select the testing binary with the `TESTING` function and then write
#tests certain TESTs defining how the test should react. Since tests are shell scripts it is possible
#tests to add some supplemental commands there to set and clean up the given test environment.
#tests
#tests HEAD^ TESTING; TESTING; set the test binary
#tests  TESTING "message" test_program
#tests
#tests Selects the test binary for the follwing tests, prints an informal message.
#tests
#tests  `message`::
#tests    message to be printed
#tests  `test_program`::
#tests    an existing program to drive the tests or a shell function
#tests
#tests ----
#tests TESTING "Testing a.out" ./a.out
#tests ----
#tests
function TESTING()
{
    echo
    echo "$1"
    echo -e "\n#### $1, $TESTFILE, $2" >>,testlog

    TESTBIN="$2"
}

#tests HEAD^ TEST; TEST; single test
#tests  TEST "title" arguments.. <<END
#tests
#tests Runs a single test
#tests
#tests  `title`::
#tests    describes this test and is also used as identifier for this test,
#tests    must be unique for all your tests
#tests  `arguments`::
#tests    the following arguments are passed to the test program
#tests  `<<END .. END`::
#tests    a list of control commands expected in and outputs is given as 'heredoc'.
#tests
#tests Each line of the test specification in the heredoc starts with an arbitary number of spaces
#tests followed by a command, followed by a colon and a space, followed by additional arguments or
#tests being an empty or comment line.
#tests
#tests
#tests HEAD+ Test Commands; commands; define expected in and outputs
#tests
#tests PARA in; in; stdin data for a test
#tests  in: text
#tests
#tests Send `text` to stdin of the test binary. If no `in:` commands are given, nothing is send to the
#tests tests input.
#tests
#tests PARA out; out; expected stdout (regex) from a test
#tests  out: regex
#tests
#tests Expect `regex` on stdout. This regexes have a 'triggering' semantic. That means it is tried to match
#tests a given regex on as much lines as possible (`.*` will match any remaining output), if the match fails,
#tests the next expected output line is tried. When that fails too the test is aborted and counted as failure.
#tests
#tests When no `out:` or `out-lit:` commands are given, then stdout is not checked, any output is ignored.
#tests
#tests PARA err; err; expected stderr (regex) from a test
#tests  out: regex
#tests
#tests Same as 'out:' but expects data on stderr. When no `err:` or `err-lit:` commands are given, then stdout is
#tests not checked, any output there is ignored.
#tests
#tests PARA out-lit; out-lit; expected stdout (literal) from a test
#tests  out-lit: text
#tests
#tests Expect `text` on stdout, must match exactly or will fail.
#tests
#tests PARA err-lit; err-lit; expected stderr (literal) from a test
#tests  err-lit: text
#tests
#tests Same as 'out-lit:' but expects data on stderr.
#tests
#tests PARA return; return; expected exit value of a test
#tests  return: value
#tests
#tests Expects `value` as exit code of the tested program. The check can be negated by prepending the value with
#tests an exclamation mark, `return: !0` expects any exist code except zero.
#tests
#tests If no `return:` command is given then a zero (success) return from the test program is expected.
#tests
#tests HEAD+ Conditional Tests; conditional tests; switch tests on conditions
#tests Sometimes tests need to be adapted to the environment/platform they are running on. This can be archived
#tests with common if-else-elseif-endif statements. This statements can be nested.
#tests
#tests PARA if; if; conditional test
#tests  if: check
#tests
#tests Executes `check` as shell command, if its return is zero (success) then the following test parts are used.
#tests
#tests PARA else; else; conditional alternative
#tests  else:
#tests
#tests If the previous `if` failed then the following test parts are included in the test, otherwise they
#tests are excluded.
#tests
#tests PARA elseif; elseif; conditional alternative with test
#tests  elseif: check
#tests
#tests Composition of else and if, only includes the following test parts if the if's and elseif's before failed
#tests and `check` succeeded.
#tests
#tests PARA endif; endif; end of conditonal test part
#tests  endif:
#tests
#tests Ends an `if` statement.
#tests
#tests HEAD+ Other Elements;;
#tests
#tests PARA msg; msg; print a diagnostic message
#tests  msg: message..
#tests
#tests Prints `message` while processing the test suite.
#tests
#tests PARA comments; comments; adding comments to tests
#tests  # anything
#tests
#tests Lines starting with the hash mark and empty lines count as comment and are not used.
#tests
function TEST()
{
        name="$1"
	shift
        rm -f ,send_stdin
        rm -f ,expect_stdout
        rm -f ,expect_stderr
        expect_return=0

        local valgrind="$valgrind"
        if [ "$VALGRINDFLAGS" = 'DISABLE' ]; then
            valgrind=
        fi

        local condstack="1"
	while read -r line; do
            local cmd="${line%%:*}:"
            local arg="${line#*: }"

            if [[ ! "$arg" ]]; then
                arg='^$'
            fi

            case $cmd in
            'if:')
                if $arg; then
                    condstack="1$condstack"
                else
                    condstack="0$condstack"
                fi
                ;;
            'elseif:')
                if [[ "${condstack:0:1}" = "0" ]]; then
                    if $arg; then
                        condstack="1${condstack:1}"
                    else
                        condstack="0${condstack:1}"
                    fi
                else
                    condstack="2${condstack:1}"
                fi
                ;;
            'else:')
                if [[ "${condstack:0:1}" != "0" ]]; then
                    condstack="0${condstack:1}"
                else
                    condstack="1${condstack:1}"
                fi
                ;;
            'endif:')
                condstack="${condstack:1}"
                ;;
            *)
                if [[ "${condstack:0:1}" = "1" ]]; then
                    case $cmd in
                    'msg:')
                        echo "MSG $arg"
                        ;;
                    'in:')
                        echo "$arg" >>,send_stdin
                        ;;
                    'out:')
                        echo "regex_cont: $arg" >>,expect_stdout
                        ;;
                    'err:')
                        echo "regex_cont: $arg" >>,expect_stderr
                        ;;
                    'out-lit:')
                        echo "literal: $arg" >>,expect_stdout
                        ;;
                    'err-lit:')
                        echo "literal: $arg" >>,expect_stderr
                        ;;
                    'return:')
                        expect_return="$arg"
                        ;;
                    '#'*|':')
                        :
                        ;;
                    *)
                        echo "UNKOWN TEST COMMAND '$cmd'" 1>&2
                        exit
                        ;;
                    esac
                fi
                ;;
            esac
        done
	echo -n "TEST $name: "
	echo -en "\nTEST $name: $* " >>,testlog

        case "$TESTMODE" in
        *FAST*)
            if grep "^TEST $name: .* FAILED" ,testlog.pre >&/dev/null; then
                MSGOK=" (fixed)"
                MSGFAIL=" (still broken)"
            elif grep "^TEST $name: .* \\(SKIPPED (ok)\\|OK\\)" ,testlog.pre >&/dev/null; then
                echo ".. SKIPPED (ok)"
                echo ".. SKIPPED (ok)" >>,testlog
                SKIPCNT=$(($SKIPCNT + 1))
                TESTCNT=$(($TESTCNT + 1))
                return
            else
                MSGOK=" (new)"
                MSGFAIL=" (new)"
            fi
            ;;
        *)
            MSGOK=""
            MSGFAIL=""
            ;;
        esac

        TESTCNT=$(($TESTCNT + 1))

        fails=0

        echo -n >,testtmp

        local CALL
        if declare -F | grep $TESTBIN >&/dev/null; then
            CALL=
        elif test -x $TESTBIN; then
            CALL="env $LIBTOOL_EX $valgrind"
        else
            CALL='-'
            echo -n >,stdout
            echo "test binary '$TESTBIN' not found" >,stderr
            ((fails+=1))
        fi

        if test "$CALL" != '-'; then
            if test -f ,send_stdin; then
                (
                    $CALL $TESTBIN "$@" <,send_stdin 2>,stderr >,stdout
                    echo $? >,return
                ) &
            else
                (
                    $CALL $TESTBIN "$@" 2>,stderr >,stdout
                    echo $? >,return
                ) &
            fi &>/dev/null
            pid=$!

            # watchdog
            ( sleep $LIMIT_TIME && kill -KILL $pid ) &>/dev/null &
            wpid=$!
            wait $pid
            return=$(<,return)
            if [[ "$return" -le 128 ]]; then
                kill -INT $wpid >&/dev/null
            fi

            if test -f ,expect_stdout; then
                grep -v "$LOGSUPPRESS" <,stdout >,tmp
                if ! compare_template ,expect_stdout ,tmp >>,cmptmp; then
                    echo "unexpected data on stdout" >>,testtmp
                    cat ,cmptmp >>,testtmp
                    ((fails+=1))
                fi
                rm ,tmp ,cmptmp
            fi

            if test -f ,expect_stderr; then
                grep -v "$LOGSUPPRESS" <,stderr >,tmp
                    cat ,tmp >>,testtmp
                if ! compare_template ,expect_stderr ,tmp >>,cmptmp; then
                    echo "unexpected data on stderr" >>,testtmp
                    cat ,cmptmp >>,testtmp
                    ((fails+=1))
                fi
                rm ,tmp ,cmptmp
            fi

            if [[ "${expect_return:0:1}" = '!' ]]; then
                if [[ "${expect_return#\!}" = "$return" ]]; then
                    echo "unexpected return value $return, expected $expect_return" >>,testtmp
                    ((fails+=1))
                fi
            else
                if [[ "${expect_return}" != "$return" ]]; then
                    echo "unexpected return value $return, expected $expect_return" >>,testtmp
                    ((fails+=1))
                fi
            fi
        fi

	if test $fails -eq 0; then
            echo ".. OK$MSGOK"
            echo ".. OK$MSGOK" >>,testlog
        else
            echo ".. FAILED$MSGFAIL";
            echo ".. FAILED$MSGFAIL" >>,testlog
            cat ,testtmp >>,testlog
            rm ,testtmp
            echo "stderr was:" >>,testlog
            cat ,stderr >>,testlog
            echo END >>,testlog
            FAILCNT=$(($FAILCNT + 1))
	    case $TESTMODE in
	    *FIRSTFAIL*)
                break 2
                ;;
            esac
        fi
}

#tests HEAD^ PLANNED; PLANNED; deactivated test
#tests  PLANNED "title" arguments.. <<END
#tests
#tests Skip a single test.
#tests
#tests  `title`::
#tests    describes this test and is also used as identifier for this test,
#tests    must be unique for all your tests
#tests  `arguments`::
#tests    the following arguments are passed to the test program
#tests  `<<END .. END`::
#tests    a list of control commands expected in and outputs is given as 'heredoc'.
#tests
#tests `PLANNED` acts as dropin replacement for `TEST`. Each such test is skipped (and counted as skipped)
#tests This can be used to specify tests in advance and activate them as soon development goes on or
#tests deactivate intentional broken tests to be fixed later.
#tests
function PLANNED()
{
	echo -n "PLANNED $1: "
	echo -en "\nPLANNED $* " >>,testlog
        echo ".. SKIPPED (planned)"
        echo ".. SKIPPED (planned)" >>,testlog
        SKIPCNT=$(($SKIPCNT + 1))
        TESTCNT=$(($TESTCNT + 1))
}

function RUNTESTS()
{
    if test \( ! "${TESTSUITES/*,*/}" \) -a "$TESTSUITES"; then
        TESTSUITES="{$TESTSUITES}"
    fi
    for t in $(eval echo "$TESTDIR/*$TESTSUITES*.tests"); do
        echo "$t"
    done | sort | uniq | {
        while read TESTFILE; do
            echo
            echo "### $TESTFILE" >&2
            if test -f $TESTFILE; then
                source $TESTFILE
            fi
        done
        echo
        if [ $FAILCNT = 0 ]; then
            echo " ... PASSED $(($TESTCNT - $SKIPCNT)) TESTS, $SKIPCNT SKIPPED"
            #rm ,testlog
        else
            echo " ... SUCCEDED $(($TESTCNT - $FAILCNT - $SKIPCNT)) TESTS"
            echo " ... FAILED $FAILCNT TESTS"
            echo " ... SKIPPED $SKIPCNT TESTS"
            echo " see ',testlog' for details"
            exit 1
        fi
    }
}

TESTSUITES="${TESTSUITES}${1:+${TESTSUITES:+,}$1}"

RUNTESTS
