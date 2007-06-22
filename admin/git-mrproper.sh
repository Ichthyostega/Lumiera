#!/bin/sh
if test "$1" = "yes"; then
	git ls-files -o | xargs rm -f
else
	(
		echo "'git-mrproper.sh yes'"
		echo "would delete following files:"
		echo
		git ls-files -o | while read file; do echo " $file"; done 
	) | less
fi
