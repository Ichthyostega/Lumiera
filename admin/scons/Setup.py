# -*- python -*-
##
## Setup.py  -  SCons build: setup, definitions and compiler flags
##

#  Copyright (C)         Lumiera.org
#    2012,               Hermann Vosseler <Ichthyostega@web.de>
#
#  This program is free software; you can redistribute it and/or
#  modify it under the terms of the GNU General Public License as
#  published by the Free Software Foundation; either version 2 of
#  the License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#####################################################################


# NOTE: scons -h for help.
# Read more about the SCons build system at: http://www.scons.org
# Basically, this script just /defines/ the components and how they
# fit together. SCons will derive the necessary build steps.


#-----------------------------------Configuration
TARGDIR      = 'target'
VERSION      = '0.pre.01'
TOOLDIR      = './admin/scons'    # SCons plugins
SCRIPTDIR    = './admin'
OPTCACHE     = 'optcache' 
CUSTOPTFILE  = 'custom-options'

# these are accessible via env.path.xxxx
srcIcon      = 'icons'
srcConf      = 'data/config'
buildExe     = '#$TARGDIR'
buildLib     = '#$TARGDIR/modules'
buildPlug    = '#$TARGDIR/modules'
buildIcon    = '#$TARGDIR/gui/icons'
buildUIRes   = '#$TARGDIR/'
buildConf    = '#$TARGDIR/config'
installExe   = '#$DESTDIR/lib/lumiera'
installLib   = '#$DESTDIR/lib/lumiera/modules'
installPlug  = '#$DESTDIR/lib/lumiera/modules'
installIcon  = '#$DESTDIR/share/lumiera/icons'
installUIRes = '#$DESTDIR/share/lumiera/'
installConf  = '#$DESTDIR/lib/lumiera/config'

#-----------------------------------Configuration
localDefinitions = locals()




import os
import sys

from SCons.Script import *

from Options import *

from Buildhelper import *
from LumieraEnvironment import *


#####################################################################

def setupBasicEnvironment():
    """ define cmdline options, build type decisions
    """
    EnsurePythonVersion(2,4)
    EnsureSConsVersion(1,0)
    
    Decider('MD5-timestamp') # detect changed files by timestamp, then do a MD5
    
    vars = Variables([OPTCACHE, CUSTOPTFILE])
    vars = defineCmdlineVariables(vars)
    
    env = LumieraEnvironment(variables=vars
                            ,toolpath = [TOOLDIR]
                            ,pathConfig = extract_localPathDefs(localDefinitions) # e.g. buildExe -> env.path.buildExe
                            ,TARGDIR  = TARGDIR
                            ,DESTDIR = '$INSTALLDIR/$PREFIX'
                            ,VERSION = VERSION
                            )
    handleVerboseMessages(env)
    
    env.Replace( CPPPATH   =["#src"]    # used to find includes, "#" means always absolute to build-root
               , CPPDEFINES=['LUMIERA_VERSION='+VERSION ]    # note: it's a list to append further defines
               , CCFLAGS='-Wall -Wextra '
               , CFLAGS='-std=gnu99' 
               )
    handleNoBugSwitches(env)
    
    env.Append(CPPDEFINES = '_GNU_SOURCE')
    appendCppDefine(env,'DEBUG','DEBUG', 'NDEBUG')
#   appendCppDefine(env,'OPENGL','USE_OPENGL')
    appendVal(env,'ARCHFLAGS','CCFLAGS')   # for both C and C++
    appendVal(env,'OPTIMIZE', 'CCFLAGS',   val=' -O3')
    appendVal(env,'DEBUG',    'CCFLAGS',   val=' -ggdb')
    
    # setup search path for Lumiera plugins
    appendCppDefine(env,'PKGLIBDIR','LUMIERA_PLUGIN_PATH=\\"$PKGLIBDIR/:ORIGIN/modules\\"'
                                   ,'LUMIERA_PLUGIN_PATH=\\"ORIGIN/modules\\"') 
    appendCppDefine(env,'PKGDATADIR','LUMIERA_CONFIG_PATH=\\"$PKGLIBDIR/:.\\"'
                                    ,'LUMIERA_CONFIG_PATH=\\"$DESTDIR/share/lumiera/:.\\"') 
    
    prepareOptionsHelp(vars,env)
    vars.Save(OPTCACHE, env)
    return env

def appendCppDefine(env,var,cppVar, elseVal=''):
    if env[var]:
        env.Append(CPPDEFINES = env.subst(cppVar) )
    elif elseVal:
        env.Append(CPPDEFINES = env.subst(elseVal))

def appendVal(env,var,targetVar,val=None):
    if env[var]:
        env.Append( **{targetVar: env.subst(val) or env[var]})


def handleNoBugSwitches(env):
    """ set the build level for NoBug. 
        Release builds imply no DEBUG
        whereas ALPHA and BETA require DEBUG
    """
    level = env['BUILDLEVEL']
    if level in ['ALPHA', 'BETA']:
        if not env['DEBUG']:
            print 'Warning: NoBug ALPHA or BETA builds requires DEBUG=yes, switching DEBUG on!'
        env.Replace( DEBUG = 1 )
        env.Append(CPPDEFINES = 'EBUG_'+level)
    elif level == 'RELEASE':
        env.Replace( DEBUG = 0 )

def handleVerboseMessages(env):
    """ toggle verbose build output """
    if not env['VERBOSE']:
        # SetOption('silent', True)
        env['CCCOMSTR'] = env['SHCCCOMSTR']   = "  Compiling    $SOURCE"
        env['CXXCOMSTR'] = env['SHCXXCOMSTR'] = "  Compiling++  $SOURCE"
        env['LINKCOMSTR']                     = "  Linking -->  $TARGET"
        env['LDMODULECOMSTR']                 = "  creating module [ $TARGET ]"



