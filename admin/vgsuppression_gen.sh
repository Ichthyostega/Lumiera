#!/bin/sh

valgrind --leak-check=yes --show-reachable=yes --gen-suppressions=all ./vgsuppression 2>&1 | sed '/==[0-9]*==/d;'
