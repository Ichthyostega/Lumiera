# -*- python -*-
##
## Buildhelper.py  -  helpers, custom builders, for SConstruct
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

import os
import sys
import glob
import fnmatch
import re
import tarfile



#
# Common Helper Functions
#
def isCleanupOperation(env):
    return env.GetOption('clean')

def isHelpRequest():
    """ this is a hack: SCons does all configure tests even if only
        the helpmessage is requested. SCons doesn't export the
        help option for retrieval by env.GetOption(), 
        so we scan the commandline directly. 
    """
    return '-h' in sys.argv or '--help' in sys.argv



def srcSubtree(env,tree,isShared=False,builder=None, **args):
    """ convienience wrapper: scans the given subtree, which is
        relative to the current SConscript, find all source files and
        declare them as Static or SharedObjects for compilation
    """
    root = env.subst(tree)  # expand Construction Vars
    if not builder:
        if isShared:
            builder = lambda f: env.SharedObject(f, **args)
        else:
            builder = lambda f: env.Object(f, **args)
        
    return [builder(f) for f in scanSrcSubtree(root)] 



SRCPATTERNS = ['*.c','*.cpp','*.cc']

def scanSrcSubtree(roots):
    """ first expand (possible) wildcards and filter out non-dirs. 
        Then scan the given subtree for source filesnames 
        (python generator function)
    """
    for root in globRootdirs(roots):
        for (dir,_,files) in os.walk(root):
            if dir.startswith('./'):
                dir = dir[2:]
            for p in SRCPATTERNS:
                for f in fnmatch.filter(files, p):
                    yield os.path.join(dir,f)



def globRootdirs(roots):
    """ helper: expand shell wildcards and filter the resulting list,
        so that it only contains existing directories
    """
    filter = lambda f: os.path.isdir(f) and os.path.exists(f)
    roots = glob.glob(roots)
    return (dir for dir in roots if filter(dir) )




def Tarball(env,location,dirs,suffix=''):
    """ Custom Command: create Tarball of some subdirs
        location: where to create the tar (optionally incl. filename.tar.gz)
        suffix: (optional) suffix to include in the tar name
        dirs: directories to include in the tar
        
        This is a bit of a hack, because we want to be able to include arbitrary dirctories,
        without creating new dependencies on those dirs. Esp. we want to tar the source tree
        prior to compiling. Solution is 
         - use the Command-Builder, but pass all target specifications as custom build vars
         - create a pseudo-target located in the parent directory (not built by default)
    """
    targetID    = '../extern-tar%s' % suffix
    versionID   = env['VERSION']
    defaultName = 'cinelerra%s_%s' % (suffix, versionID)
    nameprefix  = 'cinelerra-%s/' %  (versionID)
    location    = env.subst(location)
    dirs        = env.subst(dirs)
    return env.Command(targetID,None, createTarball, 
                       location=location, defaultName=defaultName, dirs=dirs, nameprefix=nameprefix)


def createTarball(target,source,env):
    """ helper, builds the tar using the python2.3 tarfil lib.
        This allows us to prefix all paths, thus moving the tree
        into a virtual subdirectory containing the Version number,
        as needed by common packaging systems.
    """
    name = getTarName( location = env['location']
                     , defaultName = env['defaultName'])
    targetspec = env['dirs']
    nameprefix = env['nameprefix'] or ''
    print 'Running: tar -czf %s %s ...' % (name,targetspec)
    if os.path.isfile(name):
        os.remove(name)
    tar = tarfile.open(name,'w:gz')
    for name in targetspec.split():
        tar.add(name,nameprefix+name)
    tar.close()
#
# old version using shell command:
#
#    cmd = 'tar -czf %s %s' % (name,targetspec)
#    print 'running ', cmd, ' ... '
#    pipe = os.popen (cmd)
#    return pipe.close ()




def getTarName(location, defaultName):
    """ create a suitable name for the tarball.
        - if location contains a name (*.tar.gz) then use this
        - otherwise append the defaultName to the specified dir
    """
    spec = os.path.abspath(location)
    (head,tail) = os.path.split(spec)
    if not os.path.isdir(head):
        print 'Target dir "%s" for Tar doesn\'t exist.' % head
        Exit(1)
    mat = re.match(r'([\w\.\-\+:\~]+)\.((tar)|(tar\.gz)|(tgz))', tail)
    if mat:
        name = mat.group(1)
        ext  = '.'+mat.group(2)
    else:
        ext = '.tar.gz'
        if os.path.isdir(spec):
            head = spec
            name = defaultName
        else:
            name = tail
    return os.path.join(head,name+ext)
            

