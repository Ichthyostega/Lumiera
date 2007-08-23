#!/bin/sh
find -name DIR_INFO -printf '%-30h: ' -exec head -1 {} \; | sort | cut -c 1-92
