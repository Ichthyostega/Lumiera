#!/usr/bin/python3
# coding: utf-8
##
## buildVersion.py  -  extract and possibly bump current version from Git
##

#  Copyright (C)
#    2025,            Hermann Vosseler <Ichthyostega@web.de>
#
# **Lumiera** is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the
# Free Software Foundation; either version 2 of the License, or (at your
# option) any later version. See the file COPYING for further details.
#####################################################################
'''
Build and possibly bump a current project version spec,
based on the nearest Git tag.
'''

import re
import os
import sys
import datetime
import argparse
import subprocess

#------------CONFIGURATION----------------------------
CMDNAME = os.path.basename(__file__)
TAG_PAT = 'v*.*'
VER_SEP = r'(?:^v?|\.)'
VER_NUM = r'(\w[\w\+]*)'
VER_SUB = r'(?:'+VER_SEP+VER_NUM+')'
VER_SUF = r'(?:~('+VER_NUM+VER_SUB+'?'+'))'
VER_SYNTAX = VER_SUB +VER_SUB+'?' +VER_SUB+'?' +VER_SUF+'?'
GIT = 'git'
#------------CONFIGURATION----------------------------



def parseAndBuild():
    ''' main: parse cmdline and generate version string '''
    parser = argparse.ArgumentParser (prog=CMDNAME, description='%s: %s' % (CMDNAME, __doc__)
                                     ,formatter_class=argparse.RawDescriptionHelpFormatter)

    parser.add_argument ('--bump','-b'
                        ,nargs='?'
                        ,choices=['maj','min','rev'], const='rev'
                        ,help='bump the version detected from Git (optionally bump a specific component)')
    parser.add_argument ('--suffix','-s'
                        ,help='append (or replace) a suffix (by default attached with ~)')
    parser.add_argument ('--snapshot'
                        ,action='store_true'
                        ,help='mark as development snapshot by appending ~dev.YYYYMMDDhhmm, using UTC date from HEAD commit')

    opts = parser.parse_args()
    
    version = getVersionFromGit()
    version = rebuild (version, **vars(opts))
    print (version)


def getVersionFromGit():
    get_nearest_matching_tag = 'describe --tags --abbrev=0 --match=' + TAG_PAT
    return runGit (get_nearest_matching_tag)

def getTimestampFromGit():
    get_head_author_date = 'show -s --format=%ai'
    timespec = runGit (get_head_author_date)
    timespec = datetime.datetime.fromisoformat (timespec)
    timespec = timespec.astimezone (datetime.timezone.utc) # note: convert into UTC
    return timespec.strftime ('%Y%m%d%H%M')



def rebuild (version, bump=None, suffix=None, snapshot=False):
    mat = re.fullmatch (VER_SYNTAX, version)
    if not mat:
        __FAIL ('invalid version syntax in "'+version+'"')
    
    maj = mat.group(1)
    min = mat.group(2)
    rev = mat.group(3)
    suf = mat.group(4)
    suf_idi = mat.group(5) # detail structure not used (as of 2025)
    suf_num = mat.group(6)
    
    if bump=='maj':
        maj = bumpedNum(maj)
        min = None
        rev = None
    elif bump=='min':
        min = bumpedNum(min)
        rev = None
    elif bump=='rev':
        rev = bumpedNum(rev)
    
    if snapshot:
        suf = 'dev.'+getTimestampFromGit()
    elif suffix:
        suf = suffix
    
    version = maj
    if min:
        version += '.'+min
    elif not min and rev:
        version += '.0'
    if rev:
        version += '.'+rev
    if suf:
        version += '~'+suf
    return version



def bumpedNum (verStr):
    mat = re.match (r'\d+', str(verStr))
    if not mat:
        return '1'
    numStr = mat.group(0)
    num = int(numStr) + 1
    return str(num).zfill(len(numStr))


def runGit (argStr):
    ''' run Git as system command without shell and retrieve the output '''
    argList = [GIT] + argStr.split()
    try:
        proc = subprocess.run (argList, check=True, capture_output=True, encoding='utf-8', env={'LC_ALL':'C'})
        return proc.stdout.rstrip()                                                        # Note: sanitised env
    except:
        __FAIL ('invoking git-describe')




def __ERR (*args, **kwargs):
    print (*args, file=sys.stderr, **kwargs)

def __FAIL (msg):
    __ERR ("FAILURE: "+msg)
    exit (-1)


if __name__=='__main__':
    parseAndBuild()
