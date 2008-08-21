# -*- python -*-
##
## Buildhelper.py  -  helpers, custom builders, for SConstruct
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

import os
import sys
import glob
import fnmatch
import re
import tarfile

from SCons.Action import Action



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
        
    return [builder(f) for f in scanSubtree(root)] 



SRCPATTERNS = ['*.c','*.cpp','*.cc']

def scanSubtree(roots, patterns=SRCPATTERNS):
    """ first expand (possible) wildcards and filter out non-dirs. 
        Then scan the given subtree for source filesnames 
        (python generator function)
    """
    for root in globRootdirs(roots):
        for (dir,_,files) in os.walk(root):
            if dir.startswith('./'):
                dir = dir[2:]
            for p in patterns:
                for f in fnmatch.filter(files, p):
                    yield os.path.join(dir,f)



def globRootdirs(roots):
    """ helper: expand shell wildcards and filter the resulting list,
        so that it only contains existing directories
    """
    filter = lambda f: os.path.isdir(f) and os.path.exists(f)
    roots = glob.glob(roots)
    return (dir for dir in roots if filter(dir) )




def filterNodes(nlist, removeName=None):
    """ filter out scons build nodes using the given criteria.
        removeName: if set, remove all nodes with this srcname
    """
    if removeName:
        predicate = lambda n : not fnmatch.fnmatch(os.path.basename(str(n[0])), removeName)
    else:
        predicate = lambda n : True;
    
    return filter(predicate, nlist)



def getDirname(dir):
    """ extract directory name without leading path """
    dir = os.path.realpath(dir)
    if not os.path.isdir(dir):
        dir,_ = os.path.split(dir)
    _, name = os.path.split(dir)
    return name
    


def RegisterIcon_Builder(env, renderer):
    """ Registeres Custom Builders for generating and installing Icons.
        Additionally you need to build the tool (rsvg-convert.c)
        used to generate png from the svg source using librsvg. 
    """
    renderer = __import__(renderer) # load python script for invoking the render
    renderer.rsvgPath = env.subst("$BINDIR/rsvg-convert")
    
    def invokeRenderer(target, source, env):
        source = str(source[0])
        targetdir = env.subst("$BINDIR")
        renderer.main([source,targetdir])
        return 0
        
    def createIconTargets(target,source,env):
        """ parse the SVG to get the target file names """
        source = str(source[0])
        targetdir = os.path.basename(str(target[0]))
        targetfiles = renderer.getTargetNames(source)    # parse SVG
        return (["$BINDIR/%s" % name for name in targetfiles], source)
    
    def IconCopy(env, source):
         """Copy icon to corresponding icon dir. """
         subdir = getDirname(source)
         return env.Install("$BINDIR/%s" % subdir, source)
    
     
    buildIcon = env.Builder( action = Action(invokeRenderer, "rendering Icon: $SOURCE --> $TARGETS")
                           , single_source = True
                           , emitter = createIconTargets 
                           )
    env.Append(BUILDERS = {'IconRender' : buildIcon})    
    env.AddMethod(IconCopy)



def Tarball(env,location,dirs,suffix=''):
    """ Custom Command: create Tarball of some subdirs
        location: where to create the tar (may optionally include filename.tar.gz)
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
    defaultName = 'lumiera%s_%s' % (suffix, versionID)
    nameprefix  = 'lumiera-%s/' %  (versionID)
    location    = env.subst(location)
    dirs        = env.subst(dirs)
    return env.Command(targetID,None, createTarball, 
                       location=location, defaultName=defaultName, dirs=dirs, nameprefix=nameprefix)


def createTarball(target,source,env):
    """ helper, builds the tar using the python2.3 tarfile lib.
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
            

