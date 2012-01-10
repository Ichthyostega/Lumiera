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
#-----------------------------------Configuration




import os
import sys

sys.path.append(TOOLDIR)

from Buildhelper import *
from LumieraEnvironment import *


import Setup
import Options
import Platform


#####################################################################

env = Setup.defineBuildEnvironment()
env = Platform.configure(env)



### === MAIN BUILD === ##############################################

# call subdir SConscript(s) for to define the actual build targets
SConscript(dirs=['data','src','src/tool','research','tests','doc'], exports='env')

# artifacts defined by the build targets
Import('lumiera plugins tools gui testsuite doxydoc')



# additional files to be cleaned when cleaning 'build'
env.Clean ('build', [ 'scache.conf', '.sconf_temp', '.sconsign.dblite', 'config.log' ])
env.Clean ('build', [ 'src/pre.gch' ])



### === Alias Targets === ###########################################

build = env.Alias('build', lumiera + plugins + tools +gui)

env.Alias ('doc', doxydoc)

env.Alias ('all', build + testsuite + doxydoc)
env.Default('build')
# SCons default target

env.Alias('install', gui)
env.Alias('install', '$DESTDIR')

#####################################################################
