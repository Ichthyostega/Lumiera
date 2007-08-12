#!/bin/bash 

#  Copyright (C)         CinelerraCV
#    2007,               Christian Thaeter <ct@pipapo.org>
#                        Hermann Vosseler <Ichthyostega@web.de>
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

arg0="$0"
srcdir=$(dirname "$arg0")

ulimit -S -t 1 -v 524288
valgrind=""
if [ "$VALGRINDFLAGS" = 'DISABLE' ]; then
    echo "valgrind explicit disabled"
else
    if [ "$(which valgrind)" ]; then
        valgrind="$(which valgrind) --leak-check=yes --show-reachable=yes -q $VALGRINDFLAGS"
	ulimit -S -t 10
    else
        echo "no valgrind found, go without it"
    fi
fi

echo
echo ================ ${0##*/} ================

TESTCNT=0
SKIPCNT=0
FAILCNT=0

if test -f ,testlog; then
    mv ,testlog ,testlog.pre
else
    touch ,testlog.pre
fi

date >,testlog

function TEST()
{
        name="$1"
	shift
	cat >,cmp
	echo -n "" >,out
	echo -n "TEST $name: "
	echo -en "\nTEST $name: $* " >>,testlog

        case $TESTMODE in
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
	if $valgrind $TESTBIN "$@" 2>&1 | tee ,tmp | grep -v ': \(TRACE\|INFO\|NOTICE\|WARNING\|ERR\):' | cmp ,cmp - &>/dev/null; then
            echo ".. OK$MSGOK"
            echo ".. OK$MSGOK" >>,testlog
        else
            echo ".. FAILED$MSGFAIL";
            echo ".. FAILED$MSGFAIL" >>,testlog
            grep -v ': \(TRACE\|INFO\|NOTICE\|WARNING\|ERR\):' <,tmp >,out
            diff -ua ,cmp ,out >>,testlog
            # grep 'DEBUG:\|==.*==' <,tmp >>,testlog
            cat ,tmp >>,testlog
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
    for i in $srcdir/*.tests; do
        source $i
    done
    echo
    rm ,cmp ,out ,tmp
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

function TESTING()
{
    echo
    echo "$1"
    TESTBIN=$2
}

RUNTESTS
