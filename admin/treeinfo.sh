#!/bin/sh
COLUMNS=$(tput cols)
find -name DIR_INFO -printf '%-30h: ' -exec head -1 {} \; | sort -k1,1 | cut -c 1-${COLUMNS:-92}
