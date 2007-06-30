# -*- python -*-
##
## SConstruct  -  SCons based build-sytem for Cinelerra
##

#  Copyright (C)         CinelerraCV
#    2007,               Hermann Vosseler <Ichthyostega@web.de>
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

from Buildhelper import *


# NOTE: the following code /defines/ what and how to build
#       it doesn't "do" the build. SCons will do the "doing"

env = setupBasicEnvironment()

if not isCleanupOperation(env):
    configurePlatform(env)
    
artifacts = {}
# the various things we build. 
# Each entry actually is a SCons-Node list.
# Passing these entries to other builders defines dependencies.
# 'cinelerra'   : the App
# 'plugins'     : plugin shared lib
# 'tools'       : small tool applications (e.g mpegtoc)
# 'src,tar'     : source tree as tarball (without doc)
# 'devdoc.tar'  : uml model, wiki, dev docu (no src)

definePackagingTargets(env, artifacts)
defineBuildTargets(env, artifacts)
defineInstallTargets(env, artifacts)


#####################################################################

def setupBasicEnvironment():
    ''' define cmdline options, decide build type 
    ''' 


def configurePlatform(env):
    ''' locate required libs.
        setup platform specific options.
        Abort build in case of failure.
    '''


def definePackagingTargets(env, artifacts):
    ''' build operations and targets to be done /before/ compiling.
        things like creating a source tarball or preparing a version header.
    '''


def defineBuildTargets(env, artifacts):
    ''' define the source file/dirs comprising each artifact to be built.
        setup sub-environments with special build options if necessary 
    '''


def defineInstallTargets(env, artifacts):
    ''' define install locations and cleanup after the build.
        define alias targets to trigger the installing.
    '''


