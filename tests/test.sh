#!/usr/bin/env bash
#  Copyright (C)         Lumiera.org
#    2007 - 2008,        Christian Thaeter <ct@pipapo.org>
#
#  This program is free software; you can redistribute it and/or
#  modify it under the terms of the GNU General Public License as
#  published by the Free Software Foundation; either version 2 of the
#  License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

# TESTMODE=FULL yet unimplemented
#   run all tests, PLANNED which fail count as error
#
# TESTMODE=FAST
#   run only tests which recently failed
#
# TESTMODE=FIRSTFAIL
#   stop testing on the first failure

export LC_ALL=C
NOBUG_LOGREGEX='^\(\*\*[0-9]*\*\* \)\?[0-9]\{10,\}: \(TRACE\|INFO\|NOTICE\|WARNING\|ERR\|TODO\|PLANNED\|FIXME\|DEPRECATED\|UNIMPLEMENTED\):'

arg0="$0"
srcdir="$(dirname "$arg0")"
vgsuppression_mangle='/^\(\(==\)\|\(\*\*\)[0-9]*\(==\)\|\(\*\*\)\)\|\(\(\*\*[0-9]*\*\* \)\?[0-9]\{10,\}: ECHO:\)/d;'

ulimit -S -t 5 -v 524288
valgrind=""
if [ "$VALGRINDFLAGS" = 'DISABLE' ]; then
    echo "valgrind explicit disabled"
else
    if [ "$(which valgrind)" ]; then
	ulimit -S -t 20
        if [[ -x 'vgsuppression' ]]; then
            if [[ 'vgsuppression' -nt 'vgsuppression.supp' ]]; then
                echo 'generating valgrind supression file'

                if [[ -x ".libs/vgsuppression" ]]; then
                    ./libtool --mode=execute valgrind --leak-check=yes --show-reachable=yes -q --gen-suppressions=all vgsuppression 2>&1 \
                        | sed -e "$vgsuppression_mangle" >vgsuppression.supp
                else
                    valgrind --leak-check=yes --show-reachable=yes -q --gen-suppressions=all ./vgsuppression 2>&1 \
                        | sed -e "$vgsuppression_mangle" >vgsuppression.supp
                fi
            fi
            valgrind="$(which valgrind) --leak-check=yes --show-reachable=no --suppressions=vgsuppression.supp -q $VALGRINDFLAGS"
        else
            valgrind="$(which valgrind) --leak-check=yes --show-reachable=no -q $VALGRINDFLAGS"
        fi
    else
        echo "no valgrind found, go without it"
    fi
fi

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

function compare_regex() # rxfile plainfile
{
    local regex
    local line
    local miss
    local lineno=1
    local regexno=1
    {
        IFS='' read -u 3 -r regex || return 0
        IFS='' read -u 4 -r line || { echo "no output"; return 1; }
        while true; do
            if [[ $line =~ $regex ]]; then
                IFS='' read -u 4 -r line ||
                if IFS='' read -u 3 -r regex; then
                    echo "premature end in output, expecting: '$regex':$regexno"
                    return 1
                else
                    return 0
                fi
                : $((++lineno))
                miss=0
            else
                if [[ $((++miss)) -gt 1 ]]; then
                    echo -e "'$line':$lineno\ndoes not match\n'$regex':$regexno"
                    return 1
                fi
                IFS='' read -u 3 -r regex || { echo "more output than expected: '$line':$lineno"; return 1; }
                : $((++regexno))
            fi
        done
    } 3<"$1" 4<"$2"
}


function TEST()
{
        name="$1"
	shift
        rm -f ,send_stdin
        rm -f ,expect_stdout
        rm -f ,expect_stderr
        expect_return=0

	while read -r line; do
            cmd="${line%%:*}"
            arg="${line#*:}"
            arg="${arg:1}"
            if [[ ! "$arg" ]]; then
                arg='^$'
            fi

            case $cmd in
            'in')
                echo "$arg" >>,send_stdin
                ;;
            'out')
                echo "$arg" >>,expect_stdout
                ;;
            'err')
                echo "$arg" >>,expect_stderr
                ;;
            'return')
                expect_return="$arg"
                ;;
            '#'*|'')
                :
                ;;
            *)
                echo "UNKOWN TEST COMMAND '$cmd'" 1>&2
                exit
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
            CALL="env $TESTBIN_PREFIX"
        else
            CALL='-'
            echo -n >,stdout
            echo "test binary '$TESTBIN' not found" >,stderr
            ((fails+=1))
        fi

        if test "$CALL" != '-'; then

            if test -f ,send_stdin; then
                $CALL $TESTBIN "$@" <,send_stdin 2>,stderr >,stdout
            else
                $CALL $TESTBIN "$@" 2>,stderr >,stdout
            fi &>/dev/null
            return=$?

            if test -f ,expect_stdout; then
                grep -v "$NOBUG_LOGREGEX" <,stdout >,tmp
                if ! compare_regex ,expect_stdout ,tmp >>,cmptmp; then
                    echo "unexpected data on stdout" >>,testtmp
                    cat ,cmptmp >>,testtmp
                    ((fails+=1))
                fi
                rm ,tmp ,cmptmp
            fi

            if test -f ,expect_stderr; then
                grep -v "$NOBUG_LOGREGEX" <,stderr >,tmp
                    cat ,tmp >>,testtmp
                if ! compare_regex ,expect_stderr ,tmp >>,cmptmp; then
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
    for t in $(eval echo "$srcdir/*$TESTSUITES*.tests"); do
        echo "$t"
    done | sort | uniq | {
        while read i; do
            echo
            echo "### $i" >&2
            if test -f $i; then
                source $i
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

function TESTING()
{
    echo
    echo "$1"
    echo -e "\n#### $1" >>,testlog

    if [[ -x ".libs/$2" ]]; then
        TESTBIN_PREFIX="./libtool --mode=execute $valgrind"
    else
        TESTBIN_PREFIX="$valgrind"
    fi
    TESTBIN="$2"
}

TESTSUITES="${TESTSUITES}${1:+${TESTSUITES:+,}$1}"

RUNTESTS
