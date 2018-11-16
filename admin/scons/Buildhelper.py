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

from SCons import Util



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



def srcSubtree(tree, **args):
    """ convenience wrapper: scan the given subtree, which is relative
        to the current SConscript, and find all source files.
    """
    return list(scanSubtree(tree, **args))



SRCPATTERNS = ['*.c','*.cpp','*.cc']

def scanSubtree(roots, patterns=SRCPATTERNS):
    """ first expand (possible) wildcards and filter out non-dirs. 
        Then scan the given subtree for source filenames 
        (python generator function)
    """
    for root in globRootdirs(roots):
        for (d,_,files) in os.walk(root):
            d = stripPrefix(d, './')
            for p in patterns:
                for f in fnmatch.filter(files, p):
                    yield os.path.join(d,f)



def globRootdirs(roots):
    """ helper: expand shell wildcards and filter the resulting list,
        so that it only contains existing directories
    """
    isDirectory = lambda f: os.path.isdir(f) and os.path.exists(f)
    roots = glob.glob(roots)
    return (d for d in roots if isDirectory(d) )



def findSrcTrees(location, patterns=SRCPATTERNS):
    """ find possible source tree roots, starting with the given location.
        When delving down from the initial location(s), a source tree is defined
        as a directory containing source files and possibly further sub directories.
        After having initially expanded the given location with #globRootdirs, each
        directory is examined depth first, until encountering a directory containing
        source files, which then yields a result. Especially, this can be used to traverse
        an organisational directory structure and find out all possible source trees
        to be built into packages, plugins, individual tool executables etc.
        @return: the relative path names of all source root dirs found (generator function).
    """
    for directory in globRootdirs(location):
        if isSrcDir (directory,patterns):
            yield directory
        else:
            for result in findSrcTrees (str(directory)+'/*'):
                yield result


def isSrcDir(path, patterns=SRCPATTERNS):
    """ helper: investigate the given (relative) path
        @param patterns: list of wildcards to define what counts as "source file" 
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
        predicate = lambda n : True
    
    return filter(predicate, nlist)



def getDirname (d, basePrefix=None):
    """ extract directory name without leading path,
        or without the explicitly given basePrefix
    """
    d = os.path.realpath(d)
    if not os.path.isdir(d):
        d,_ = os.path.split(d)
    if basePrefix:
        basePrefix = os.path.realpath(basePrefix)
        name = stripPrefix(str(d), basePrefix)
    else:
        _, name = os.path.split(d)
    return name



def stripPrefix(path, prefix):
    if path.startswith(prefix):
        path = path[len(prefix):]
    return path



def createPlugins(env, directory, **kw):
    """ investigate the given source directory to identify all contained source trees.
        @return: a list of build nodes defining a plugin for each of these source trees.
    """
    return [env.LumieraPlugin( getDirname(tree) 
                             , srcSubtree(tree)
                             , **kw
                             )
            for tree in findSrcTrees(directory)
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
    """ a set of properties with record style access.
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
            if (k.startswith('src') or k.startswith('build') or k.startswith('install')) and Util.is_String(v):
                v = v.strip()
                if not v.endswith('/'): v += '/'
                yield (k,v)
                
    return dict(relevantPathDefs(localDefs))
