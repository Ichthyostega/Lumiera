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
# This script /defines/ the components and how they fit together.
# SCons will derive dependencies and the necessary build steps.
# Read more about the SCons build system at: http://www.scons.org


# NOTE: Lumiera SCons extension modules and plugins
#--------------------------------------------------
import sys
sys.path.append('./admin/scons')
#--------------------------------------------------


import Setup
import Options
import Platform

from Buildhelper import *
from LumieraEnvironment import *



#####################################################################

env = Setup.defineBuildEnvironment()         # dirs & compiler flags
env = Platform.configure(env)                # library dependencies



### === MAIN BUILD === ##############################################

# call subdir SConscript(s) to define the actual build targets...
SConscript(dirs=['data','src','src/tool','research','tests','doc'], exports='env')




# additional files to be cleaned when cleaning 'build'
env.Clean ('build', [ 'scache.conf', '.sconf_temp', '.sconsign.dblite', 'config.log' ])
env.Clean ('build', [ 'src/pre.gch' ])



### === Alias Targets === ###########################################

# pick up the targets defined by the sub SConscripts
Import('lumiera plugins tools gui testsuite doxydoc')

build = env.Alias('build', lumiera + plugins + tools + gui)
env.Default('build')
# SCons default target


env.Alias ('all', build + testsuite + doxydoc)
env.Alias ('doc', doxydoc)
env.Alias ('none')

env.Alias('install', gui)
env.Alias('install', '$DESTDIR')

#####################################################################
