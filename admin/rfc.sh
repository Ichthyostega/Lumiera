#!/bin/bash
shopt -s extglob


#
# RFC's are created from ./doc/devel/template/new_rfc.sh and stored in
# ./doc/devel/rfc/ . There are directories for the various states
# ./doc/devel/rfc_final, ./doc/devel/rfc_pending, ./doc/devel/rfc_parked,
# ./doc/devel/rfc_dropped . Which contain symlinks back to ../rfc/ 
#


function usage()
{
    grep -v '^// ' <<"EOF" | less -F
Lumiera RFC maintenance script
==============================
// Note: the source of this documentation is maintained
//       directly admin/rfc.sh in its usage() function
//       edit it only there and then regenerate
//       doc/devel/technical/infra/ with:
//       ./admin/rfc.sh help >doc/technical/infra/rfcsh.txt

Usage
-----

 ./admin/rfc.sh <command> [options]

Commands (with <mandatory> and [optional] parameters)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 find <rfcs> [regex]::    List all matching RFC's (matching `regex`)
 show <rfcs> [regex]::    Read RFC's (matching `regex`)
 create <title>::         Create a new RFC
 edit <rfc> [chapter]::   Edit RFC at chapter
 asciidoc <rfc>::         pass the rfc.txt to `asciidoc` command
 comment <rfc>::          Add a new comment to a RFC
 draft <rfc>::            Change RFC to Draft state
 final <rfc>::            Change RFC to Final state
 park <rfc>::             Change RFC to Parked state
 drop <rfc>::             Change RFC to Dropped state
 supersede <rfc> <new>::  Supersede RFC with a new RFC
 discard <rfc>::          Delete an RFC
 help::                   Show this help
 process::                Do automatic maintenance work
 wrap <rfcs>::            canonical reformatting

Option types
~~~~~~~~~~~~

 title::                Quoted string used as RFC title
 rfc::                  Name of the RFC, smart matched, unique
 rfcs::                 Name of the RFC, smart matched, multiple
 regex::                Regex matched against the content of a RFC
 chapter::              Heading of a section


Smart matching
--------------

RFC names don't need to be given exactly, they use a globbing pattern.
This is:

 * case insensitive
 * whitespaces are ignored
 * `*` stands for any number of parameters
 * `?` is any single character
 * when starting with `/` they are matched against the begin of the name
 * some regex operators work too

`find` and `show` can operate on many matches so the given rfc name doesn't
need to be unique. The other commands will complain when the RFC name given
doesn't resolve to one unique RFC.

When `less` presents multiple files one can go forth and back with the `:n`
and `:p` commands.

The tile for `create` should be a normal document title. Possibly quoted
since it may contain spaces and not too long. The filename is this title
in CamelCase with all spaces and special characters removed.

Chapter machching single lines containing this word, special asciidoc
comments in the form `//word:.*` and asciidoc block attributes `[word.*]`
on a single line. When a chapter pattern is not unique, the last one is
picked.

rfc.sh executes git add/rm/mv commands, but never commits. One should do a
commit as soon he finished editing.

EOF
}


function camel_case()
{
    sed -e 's/[^[:alnum:]]/ /g;s/ \+\(.\)/\U\1/g' <<<" $1"
}



function untabify()
{
    local in="$1"
    local tmp
    local out

    while [[ "$in" ]]; do
        tmp="${in%%$'\t'*}"
        in="${in:${#tmp}+1}"
        out="$out$tmp        "
        out="${out:0:${#out}/8*8}"
    done
    out="${out%%*( )}"

    echo "$out"
}



function word_wrap()
{
    local linelength="${1:-80}"
    local indent=""
    local text
    local tab=$'\t'
    local rest
    local oldindent
    local coderx='//|/\*|#'
    local meld=yes

    while IFS='' read -r line; do
        line="$(untabify "$line")"
        # remove DOS linebreaks
        line="${line//$'\r'/}"

        text="${line##*([![:alnum:]])}"
        oldindent="$indent"
        indent="${line:0:${#line}-${#text}}"
        if [[ "$text" ]]; then

            # avoid melding text after // /* and # comments
            if [[ "$indent" =~ $coderx ]]; then
                meld=no
            fi

            if [[ $meld = yes && ! "${indent// /}" && ${#oldindent} -eq ${#indent} ]]; then
                line="${rest:+$rest }$text"
            else
                if [[ "$rest" ]]; then
                    echo "$rest"
                fi
            fi
            indent="${indent//?/ }"

            while [[ "${#line}" -ge "${linelength}" ]]; do
                local wrapped
                local tmpline="${line:0:$linelength}"
                wrapped="${tmpline% *}"
                if [[ "${#wrapped}" -gt "${#indent}" ]]; then
                    line="${indent}${line:${#wrapped}+1}"
                else
                    wrapped="$line"
                    line=''
                fi
                echo "$wrapped"
            done

            rest="${line}"
        else
            meld=yes
            if [[ "$rest" ]]; then
                echo "$rest"
                rest=""
            fi
            echo "$line"
        fi
    done
    if [[ "$rest" ]]; then
        echo "$rest"
    fi
}


function smart_wrap()
{
    # calling on a file
    word_wrap $2 <"$1" >"$1.$$.tmp"
    if ! cmp "$1" "$1.$$.tmp" &>/dev/null ; then
        cat "$1.$$.tmp" >"$1"
        git add "$1"
    fi
    rm "$1.$$.tmp"
}



function find_rfc()
{
    # find rfc by shortname
    local file
    local match="$1"
    match="${match// /}"
    match="${match//./\.}"
    match="${match//\?/.}"
    match="${match//\*/.*}"

    local globstate=$(shopt -p nocasematch)
    shopt -s nocasematch

    for file in $(find ./doc/devel/rfc/ -name '*.txt');
    do
        local name="/${file##*/}"
        if [[ "$name" =~ $match ]]; then
            echo "$file"
        fi
    done
    $globstate
}



function find_chapter()
{
    # find chapter
    local file="$1"
    local chapter="$2"
    local found=$(grep -n -i "^\($chapter\|//$chapter:.*\|\[$chapter.*\]\)\$" "$file" | tail -1)
    if [[ "$found" ]]; then
        echo "${found%%:*}"
    fi
}



function process_file()
{
    local file="$1"
    local path="${1%/*}"
    local basename="${1##*/}"
    local linkdest="$path"
    local state=$(grep '^\*State\* *' "$file")

    case "$state" in
    *Final*)
        linkdest="./doc/devel/rfc_final"
        ;;
    *Idea*|*Draft*)
        linkdest="./doc/devel/rfc_pending"
        ;;
    *Parked*)
        linkdest="./doc/devel/rfc_parked"
        ;;
    *Dropped*)
        linkdest="./doc/devel/rfc_dropped"
        ;;
    *)
        echo "Unknown State: '$state'" >&2
        exit 1
    esac

    local oldpath
    for oldpath in ./doc/devel/rfc_*/$basename; do :; done

    if [[ -h "$oldpath" ]]; then
        if [[ "$oldpath" != "$linkdest/$basename" ]]; then
            git mv "$oldpath" "$linkdest/$basename"
        fi
    elif [[ ! -s "$linkdest/$basename" ]]; then
        ln -s "../rfc/$basename" "$linkdest/"
        git add "$linkdest/$basename"
    fi
}


function edit()
{
    # filename lineoffset chapter
    EDITOR="${EDITOR:-$(git config --get core.editor)}"
    EDITOR="${EDITOR:-$VISUAL}"

    if [ -z "$EDITOR" ]; then
        echo -e "\nFATAL\n\$EDITOR undefined\n\n"
        exit -1
    fi

    local file="$1"
    local line=0

    if [[ "$3" ]]; then
        line=$(find_chapter "$file" "$3")
    fi

    $EDITOR +$(($line+${2:-1})) $file
}


function unique_name()
{
    local files=($(find_rfc "$1"))

    if [[ ${#files[*]} -gt 1 ]]; then
        echo "multiple matches:" >&2
        (
            IFS=$'\n'
            echo "${files[*]}" >&2
        )
    elif [[ ${#files[*]} -eq 0 ]]; then
        echo "no matches" >&2
    else
        echo ${files[0]}
    fi
}


function add_comment()
{
    local name="$1"
    local nl=$'\n'
    local comment="//edit comment$nl$nl$(git config --get user.name):: '$(date +%c)' ~<$(git config --get user.email)>~$nl"

    ed "$name" 2>/dev/null <<EOF
/endof_comments:/-1i
$comment
.
wq
EOF

    edit "$name" -4 "endof_comments"
}

function edit_state()
{
    local name="$1"
    local state="$2"
    local comment="$3"

    ed "$name" 2>/dev/null <<EOF
/^\*State\*/s/\(\*State\* *_\).*_/\1${state}_/
wq
EOF

    if [[ "$comment" ]]; then

        ed "$name" 2>/dev/null <<EOF
/endof_comments:/-1i
$comment
.
wq
EOF

    fi
}


function change_state()
{
    #rfcname state
    local name="$1"
    local state="$2"

    local nl=$'\n'
    local comment=".State -> $state$nl//add reason$nl$nl$(git config --get user.name):: '$(date +%c)' ~<$(git config --get user.email)>~$nl"
    edit_state "$name" "$state" "$comment"
    edit "$name" -4 "endof_comments"
    process_file "$name"
}


command="$1"
shift

case "$command" in
process)
    # for all rfc's
    for file in $(find ./doc/devel/rfc -name '*.txt');
    do
        echo "process $file"
        process_file "$file"
    done
    :
    ;;
search)
    grep -r -C3 -n "$1" ./doc/devel/rfc | less -F
    ;;
find|list|ls)
    if [[ "$2" ]]; then
        find_rfc "$1" | xargs grep -i -C3 -n "$2"
    else
        find_rfc "$1"
    fi
    ;;
show|less|more)
    if [[ "$2" ]]; then
        less $(find_rfc "$1" | xargs grep -i -l "$2")
    else
        less $(find_rfc "$1")
    fi
    ;;
create)
    TITLE="$@"
    name=$(camel_case "$TITLE")
    if [[ -f "./doc/devel/rfc/${name}.txt" ]]; then
        echo "$name.txt exists already"
    else
        source ./doc/devel/template/new_rfc.sh >"./doc/devel/rfc/${name}.txt"
        edit "./doc/devel/rfc/${name}.txt" 2 abstract
        git add "./doc/devel/rfc/${name}.txt"
        process_file "./doc/devel/rfc/${name}.txt"
    fi
    ;;
edit)
    name=$(unique_name "$1")
    if [[ "$name" ]]; then
        edit "${name}" 2 "$2"
        process_file "$name"
    fi
    ;;
asciidoc)
    name=$(unique_name "$1")
    if [[ "$name" ]]; then
        asciidoc "${name}"
    fi
    ;;
draft)
    name=$(unique_name "$1")
    if [[ "$name" ]]; then
        change_state "$name" Draft
    fi
    ;;
park)
    name=$(unique_name "$1")
    if [[ "$name" ]]; then
        change_state "$name" Parked
    fi
    ;;
final)
    name=$(unique_name "$1")
    if [[ "$name" ]]; then
        change_state "$name" Final
    fi
    ;;
supersede)
    name=$(unique_name "$1")
    newname=$(unique_name "$2")
    newname="${newname##*/}"
    newname="${newname%.txt}"
    if [[ "$name" && "$newname" ]]; then
        change_state "$name" "Superseded by $newname"
    fi
    ;;
drop)
    name=$(unique_name "$1")
    if [[ "$name" ]]; then
        change_state "$name" Dropped
    fi
    ;;
comment)
    name=$(unique_name "$1")
    if [[ "$name" ]]; then
        add_comment "${name}"
        process_file "$name"
    fi
    ;;
discard)
    name=$(unique_name "$1")

    if [[ "$name" ]]; then
        for link in ./doc/devel/rfc_*/${name##*/}; do :; done

        if [[ -h "$link" ]]; then
            git rm -f "${link}" || rm "${link}"
        fi

        git rm -f "${name}" || rm "${name}"
    fi
    ;;
wrap)
    find_rfc "$1" | while read file; do
        smart_wrap "$file" 80
        process_file "$file"
    done
    ;;
smart_wrap)
    # multipurpose smart wrap
    smart_wrap "$1" ${2:-80}
    ;;
help|*)
    usage
    ;;
esac



