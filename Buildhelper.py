# -*- python -*-
##
## Buildhelper.py  -  helpers, custom builders, for SConstruct
##
from fnmatch import fnmatch

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

import os
import sys
import fnmatch

#
# Common Helper Functions
#
def isCleanupOperation(env):
    return env.GetOption('clean')

def isHelpRequest():
    ''' this is a hack: SCons does all configure tests even if only
        the helpmessage is requested. SCons doesn't export the
        help option for retrieval by env.GetOption(), 
        so we scan the commandline directly. 
    '''
    return '-h' in sys.argv or '--help' in sys.argv



def srcSubtree(env,tree,isShared=False, **args):
    ''' convienience wrapper: scans the given subtree, which is
        to be located within $SRCDIR, find all source files and
        declare them as Static or SharedObjects for compilation
    '''
    root = env.subst('$SRCDIR/%s' % tree)  # expand $SRCDIR
    if isShared:
        builder = lambda f: env.SharedObject(f, **args)
    else: 
        builder = lambda f: env.Object(f, **args)
        
    return [builder(f) for f in scanSrcSubtree(root)] 



SRCPATTERNS = ['*.c','*.Cpp','*.cc']

def scanSrcSubtree(root):
    ''' scan the given subtree for source filesnames 
        (python generator function)
    '''
    for (dir,_,files) in os.walk(root):
        if dir.startswith('./'):
            dir = dir[2:]
        for p in SRCPATTERNS:
            for f in fnmatch.filter(files, p):
                yield os.path.join(dir,f)
