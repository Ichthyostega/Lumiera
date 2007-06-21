#!/bin/sh
find -name DIR_INFO -printf '%-40h: ' -exec head -1 {} \; | sort | cut -c 1-92
