# coding: utf-8
##
## ToolCCache.py  -  SCons tool for integrating with CCache compiler cache
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
        C/C++ compiler invocations through CCache, while using
        CCache config variables found in the os.environment.
    """
    if not exists(env): return
    
    assert env['CCACHE']
    if not env['CCACHE'] in env['CC']:
        env['CC'] = env.subst('$CCACHE $CC')
    if not env['CCACHE'] in env['CXX']:
        env['CXX'] = env.subst('$CCACHE $CXX')
    print env.subst("* Build using $CCACHE")
    
    for i in ['HOME'
             ,'CCACHE_DIR'
             ,'CCACHE_TEMPDIR'
             ,'CCACHE_LOGFILE'
             ,'CCACHE_PATH'
             ,'CCACHE_CC'
             ,'CCACHE_CPP2'
             ,'CCACHE_PREFIX'
             ,'CCACHE_DISABLE'
             ,'CCACHE_READONLY'
             ,'CCACHE_NOSTATS'
             ,'CCACHE_NLEVELS'
             ,'CCACHE_HARDLINK'
             ,'CCACHE_RECACHE'
             ,'CCACHE_UMASK'
             ,'CCACHE_HASHDIR'
             ,'CCACHE_UNIFY'
             ,'CCACHE_EXTENSION'
             ]:
        if os.environ.has_key(i) and not env.has_key(i):
            env['ENV'][i] = os.environ[i]



def exists(env):
    """ Ensure CCache is available.
    """
    return checkCommandOption(env, 'CCACHE', cmdName='ccache')

