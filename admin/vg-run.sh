#!/bin/bash
#
# vg-run.sh - convenience wrapper script to run an executable with valgrind
#             using "typical options"; output goes to a logfile, which is
#             fed to less afterwards.
#
#             a valgrind suppression file is generated automatically, in
#             case there is an executable "vgsuppression" located in the same
#             directory as the target executable
#
# Ichthyo 8/2007
#
LOGFILE=,valgrind.log
SUPPRESS=vgsuppression
VALGRINDFLAGS=${VALGRINDFLAGS:---leak-check=yes --show-reachable=yes --demangle=yes}

EXECUTABLE=$1
if [[ ! -x $EXECUTABLE ]]; then
	echo -e "ERROR: executable \"$EXECUTABLE\" not found.\n\n"
	exit -1
fi


PATHPREFIX=${EXECUTABLE%/*}
SUPPRESS="$PATHPREFIX/$SUPPRESS"

if [[ -x $SUPPRESS ]]; then
	if [[ $SUPPRESS -nt $SUPPRESS.supp ]]; then
		echo 'generating valgrind supression file...'

		valgrind $VALGRINDFLAGS -q --gen-suppressions=all $SUPPRESS 2>&1 \
			| awk '/^{/ {i = 1;} /^}/ {i = 0; print $0;} {if (i == 1) print $0;}' >$SUPPRESS.supp
		echo 'done.'
	fi
      SUPPRESSIONFLAG="--suppressions=$SUPPRESS.supp"
else
	echo 'no suppression.'
fi

echo "running......$@"
        
valgrind $VALGRINDFLAGS --log-file=$LOGFILE.%p $SUPPRESSIONFLAG $@ &
PID=$!
wait $PID
less $LOGFILE.$PID

