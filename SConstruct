# -*- python -*-
##
## SConstruct  -  SCons based build-system for Lumiera
##

#  Copyright (C)         Lumiera.org
#    2008,               Hermann Vosseler <Ichthyostega@web.de>
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
TOOLDIR      = './admin/scons'    # SCons plugins
SCRIPTDIR    = './admin'
#-----------------------------------Configuration




import os
import sys

sys.path.append(TOOLDIR)
sys.path.append(SCRIPTDIR)

from Buildhelper import *
from LumieraEnvironment import *


import Setup
import Options
import Platform


#####################################################################


def defineBuildTargets(env):
    """ define the source file/dirs comprising each artifact to be built.
        setup sub-environments with special build options if necessary.
        We use a custom function to declare a whole tree of srcfiles. 
    """
    
    # define Icons to render and install
    vector_icon_dir      = env.subst(env.path.srcIcon+'svg')
    prerendered_icon_dir = env.subst(env.path.srcIcon+'prerendered')
    icons   = ( [env.IconRender(f)   for f in scanSubtree(vector_icon_dir,      ['*.svg'])]
              + [env.IconResource(f) for f in scanSubtree(prerendered_icon_dir, ['*.png'])]
              )
    
    #define Configuration files to install
    config  = ( env.ConfigData(env.path.srcConf+'setup.ini', targetDir='$ORIGIN')
              + env.ConfigData(env.path.srcConf+'dummy_lumiera.ini')
              )
    
    # call subdir SConscript(s) for independent components
    SConscript(dirs=['src','src/tool','research','tests'], exports='env icons config')



def definePostBuildTargets(env):
    """ define further actions after the core build (e.g. Documentation).
        define alias targets to trigger the installing.
    """
    Import('lumiera plugins tools gui testsuite')
    build = env.Alias('build', lumiera + plugins + tools +gui)
    
    # additional files to be cleaned when cleaning 'build'
    env.Clean ('build', [ 'scache.conf', '.sconf_temp', '.sconsign.dblite', 'config.log' ])
    env.Clean ('build', [ 'src/pre.gch' ])
    
    doxydoc = env.Doxygen('doc/devel/Doxyfile')
    env.Alias ('doc', doxydoc)
    env.Clean ('doc', doxydoc + ['doc/devel/,doxylog','doc/devel/warnings.txt'])
    
    env.Alias ('all', build + testsuite + doxydoc)
    env.Default('build')
    # SCons default target


def defineInstallTargets(env):
    """ define additional artifacts to be installed into target locations.
        @note: we use customised SCons builders defining install targets 
               for all executables automatically. see LumieraEnvironment.py
    """
    Import('lumiera plugins tools gui testsuite')
    
    env.SymLink('$DESTDIR/bin/lumiera',env.path.installExe+'lumiera','../lib/lumiera/lumiera')
#   env.Install(dir = '$DESTDIR/share/doc/lumiera$VERSION/devel', source=doxydoc)
    
    env.Alias('install', gui)
    env.Alias('install', '$DESTDIR')

#####################################################################





### === MAIN === ####################################################

env = Setup.setupBasicEnvironment()

if not (isCleanupOperation(env) or isHelpRequest()):
    env = Platform.configurePlatform(env)
    
# the various things we build. 
# Each entry actually is a SCons-Node list.
# Passing these entries to other builders defines dependencies.
# 'lumiera'     : the App
# 'gui'         : the GTK UI (plugin)
# 'plugins'     : plugin shared lib
# 'tools'       : small tool applications (e.g mpegtoc)

defineBuildTargets(env)
definePostBuildTargets(env)
defineInstallTargets(env)

