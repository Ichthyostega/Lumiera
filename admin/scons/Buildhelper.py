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

from SCons import Util
from SCons.Action import Action



#
# Common Helper Functions
#
def isCleanupOperation(env):
    return env.GetOption('clean')

def isHelpRequest():
    """ this is a hack: SCons does all configure tests even if only
        the help message is requested. SCons doesn't export the
        help option for retrieval by env.GetOption(), 
        so we scan the commandline directly. 
    """
    return '-h' in sys.argv or '--help' in sys.argv



def srcSubtree(env,tree,isShared=True,builder=None, **args):
    """ convenience wrapper: scans the given subtree, which is
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
        Then scan the given subtree for source filenames 
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



def findSrcTrees(location, patterns=SRCPATTERNS):
    """ find possible source tree roots, starting with the given location.
        When delving down from the initial location(s), a source tree is defined
        as a directory containing source files and possibly further sub directories.
        After having initially expanded the given location with #globRootdirs, each
        directory is examined depth first, until encountering a directory containing
        source files, which then yields a result. Especially, this can be used to traverse
        an organisational directory structure and find out all possible source trees of
        to be built into packages, plugins, individual tool executables etc.
        @return: the relative path names of all source root dirs found (generator function).
    """
    for dir in globRootdirs(location):
        if isSrcDir(dir,patterns):
            yield dir
        else:
            for result in findSrcTrees(str(dir)+'/*'):
                yield result


def isSrcDir(path, patterns=SRCPATTERNS):
    """ helper: investigate the given (relative) path
        @param patterns: list of wildcards defining what counts as "source file" 
        @return: True if it's a directory containing any source file
    """
    if not os.path.isdir(path):
         return False
    else:
        for p in patterns:
            if glob.glob(path+'/'+p):
                return True



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



def createPlugins(env, dir, **kw):
    """ investigate the given source directory to identify all contained source trees.
        @return: a list of build nodes defining a plugin for each of these source trees.
    """
    return [env.LumieraPlugin( getDirname(tree) 
                             , srcSubtree(env, tree)
                             , **kw
                             )
            for tree in findSrcTrees(dir)
           ]



def checkCommandOption(env, optID, val=None, cmdName=None):
    """ evaluate and verify an option, which may point at a command.
        besides specifying a path, the option may read True, yes or 1,
        denoting that the system default for this command should be used.
        @return: True, if the key has been expanded and validated,
                 False, if this failed and the key was removed
    """
    if not val:
        if not env.get(optID): return False
        else:
            val = env.get(optID)
    
    if val=='True' or val=='true' or val=='yes' or val=='1' or val == 1 :
        if not cmdName:
            print "WARNING: no default for %s, please specify a full path." % optID
            del env[optID]
            return False
        else:
            val = env.WhereIs(cmdName)
            if not val:
                print "WARNING: %s not found, please specify a full path" % cmdName
                del env[optID]
                return False
    
    if not os.path.isfile(val):
        val = env.WhereIs(val)
    
    if val and os.path.isfile(val):
        env[optID] = val
        return True
    else:
        del env[optID]
        return False




class Record(dict):
    """ a set of properties with map style access.
        Record is a dictionary, but the elements can be accessed
        conveniently as if they where object fields
    """
    def __init__(self, defaults=None, **props):
        if defaults:
            defaults.update(props)
            props = defaults
        dict.__init__(self,props)
        
    def __getattr__(self,key):
        if key=='__get__' or key=='__set__':
            raise AttributeError
        return self.setdefault(key)
    
    def __setattr__(self,key,val):
        self[key] = val


def extract_localPathDefs (localDefs):
    """ extracts the directory configuration values.
        For sake of simplicity, paths and directories are defined
        immediately as global variables in the SConstruct. This helper
        extracts from the given dict the variables matching some magical
        pattern and returns them wrapped into a Record for convenient access
    """
    def relevantPathDefs (mapping):
        for (k,v) in mapping.items():
            if k.startswith('build') or k.startswith('install') and Util.is_String(v):
                v = v.strip()
                if not v.endswith('/'): v += '/'
                yield (k,v)
                
    return dict(relevantPathDefs(localDefs))
