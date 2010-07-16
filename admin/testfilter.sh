#!/bin/sh
#
# Little hack to filter nobug logging output producing something suiteable for testsuites
#

gawk --posix -f /dev/fd/3 3<<"EOF"
BEGIN {
    FS = ": "
    toescape = "[]().*{}"
    prev = ""
    LOGSUPPRESS="TRACE|INFO|NOTICE|WARNING|ERR|TODO|PLANNED|FIXME|DEPRECATED|UNIMPLEMENTED|RESOURCE_ANNOUNCE|RESOURCE_ENTER|RESOURCE_STATE|RESOURCE_LEAVE"
}

function escape(string) {
    for (i = 1; i <= length(toescape); ++i)
        gsub("\\"substr(toescape, i, 1), "\\"substr(toescape, i, 1), string)

    return string
}

function linerest(start, end) {
    result = $start

    for (i = start+1; i <= end; i++)
    {
        result = result": "$i
    }
    return result
}


function remove_duplicate_spaces(rest) {
    return "foo"rest"bar"
}


/[0-9]{10}: [A-Z]*: / {
    rest = linerest(6, NF);

    rest = escape(rest)
    gsub(/0x[0-9a-f]+/,"0x[0-9a-f]+", rest)

    new = $2 ": .*: " $4 ": " $5 ": " rest

    if (new != prev && !(new ~ LOGSUPPRESS))
    {
        print "err: " new
        prev = new
    }
}

EOF
