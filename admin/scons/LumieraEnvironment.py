# -*- python -*-
##
## LumieraEnvironment.py  -  custom SCons Environment
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


import SCons
import SCons.SConf
from SCons.Environment import Environment

from Buildhelper import *



class LumieraEnvironment(Environment):
    """ Custom SCons build environment for Lumiera
        This allows us to carry structured config data without
        using global vars. Idea inspired by Ardour. 
    """
    def __init__(self,*args,**kw):
        Environment.__init__ (self,*args,**kw)
        self.libInfo = {}
    
    def Configure (self, *args, **kw):
        kw['env'] = self
        return apply(LumieraConfigContext, args, kw)
    
    def mergeConf (self,other):
        """ extract the library/compiler flags from other Environment.
            Optionally accepts a list or just sting(s) representing keys
            in our own libInfo Dictionary
        """
        if isinstance(other, list):
            for elm in other:
                self.mergeConf(elm)
        elif isinstance(other, str):
            if other in self.libInfo:
                self.mergeConf(self.libInfo[other])
        else:
            self.Append (LIBS = other.get ('LIBS',[]))
            self.Append (LIBPATH = other.get ('LIBPATH', []))    
            self.Append (CPPPATH = other.get('CPPPATH', []))
            self.Append (LINKFLAGS = other.get('LINKFLAGS', []))
        
        return self
    
    
    def addLibInfo (self, libID, minVersion=0):
        """ use pkg-config to create an Environment describing the lib.
            Don't add this defs to the current Environment, rather store
            them in the libInfo Dictionary. 
        """
        minVersion = str(minVersion)
        if 0 != os.system('pkg-config --print-errors --exists "%s >= %s"' % (libID,minVersion)):
            print "Problems configuring the Library %s (>= %s)" % (libID,minVersion)
            return False
        
        self.libInfo[libID] = libInfo = LumieraEnvironment() 
        libInfo.ParseConfig ('pkg-config --cflags --libs '+ libID )
        return libInfo
    
    def Glob (self, pattern):
        """ temporary workaround; newer versions of SCons provide this as a global function """
        pattern = self.subst(pattern)
        return glob.glob(pattern)
    
    def AddMethod (self, function):
        """ temporary workaround; newer versions of SCons provide this as a global function """
        self.__dict__[function.__name__] = function.__get__(self)
        
        
#### temporary pre 1.0 SCons compatibility hack ####
_ver = map(int, SCons.__version__.split('.')[:2])
_old = (_ver[0]<1 and _ver[1]<97)
if _old:
    ConfigBase = SCons.SConf.SConf
else:
    ConfigBase = SCons.SConf.SConfBase
    del LumieraEnvironment.Glob
    del LumieraEnvironment.AddMethod
    # use the official impl present since SCons 0.98
    # use the new name of the config context base class



class LumieraConfigContext(ConfigBase):
    """ Extends the SCons Configure context with some convenience methods
    """
    def __init__(self, *args,**kw):
        ConfigBase.__init__(self,*args,**kw)
    
    def CheckPkgConfig (self, libID, minVersion=0):
        print "Checking for library configuration: %s " % libID
        # self.Message(self,"Checking for library configuration: %s " % libID)
        return self.env.addLibInfo (libID, minVersion)


