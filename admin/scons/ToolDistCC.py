# coding: utf-8
##
## ToolDistCC.py  -  SCons tool for distributed compilation using DistCC
##

#  Copyright (C)      Lumiera.org and FreeOrion.org
#    2008,            Hermann Vosseler <Ichthyostega@web.de>
#
# **Lumiera** is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the
# Free Software Foundation; either version 2 of the License, or (at your
# option) any later version. See the file COPYING for further details.
#####################################################################

# This SCons builder was extracted from http://www.freeorion.org/
# FreeOrion is an open-source platform-independent galactic conquest game
#
# history: 12/2008 adapted for Lumiera build system


import os
from Buildhelper import *


def generate(env):
    """ Modify the environment such as to redirect any
        C/C++ compiler invocations through DistCC. Additionally
        pull in the environment config variables used by DistCC
    """
    if not exists(env): return
    
    assert env['DISTCC']
    if not env['DISTCC'] in env['CC']:
        env['CC'] = env.subst('$DISTCC $CC')
    if not env['DISTCC'] in env['CXX']:
        env['CXX'] = env.subst('$DISTCC $CXX')
    print env.subst("* Build using $DISTCC")
    for i in ['HOME'
             ,'DISTCC_HOSTS'
             ,'DISTCC_VERBOSE'
             ,'DISTCC_FALLBACK'
             ,'DISTCC_LOG'
             ,'DISTCC_MMAP'
             ,'DISTCC_SAVE_TEMPS'
             ,'DISTCC_TCP_CORK'
             ,'DISTCC_SSH'
             ]:
        if os.environ.has_key(i) and not env.has_key(i):
            env['ENV'][i] = os.environ[i]


def exists(env):
    """ Ensure DistCC exists.
    """
    return checkCommandOption(env, 'DISTCC', cmdName='distcc')

