# coding: utf-8
##
## LumieraEnvironment.py  -  custom SCons Environment
##

#  Copyright (C)
#    2008,            Hermann Vosseler <Ichthyostega@web.de>
#
# **Lumiera** is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the
# Free Software Foundation; either version 2 of the License, or (at your
# option) any later version. See the file COPYING for further details.
#####################################################################


from os import path

import SCons.SConf
from SCons.Action import Action
from SCons.Environment import Environment

from Buildhelper import *



class LumieraEnvironment(Environment):
    """ Custom SCons build environment for Lumiera
        This allows us to carry structured config data without
        using global vars. Idea inspired by Ardour. 
    """
    def __init__(self, buildSetup, buildVars, **kw):
        kw.update(VERSION = buildSetup.VERSION
                 ,TARGDIR = buildSetup.TARGDIR
                 ,DESTDIR = '$INSTALLDIR/$PREFIX'
                 ,toolpath = [buildSetup.TOOLDIR ]
                 ,variables = buildVars
                 )
        Environment.__init__ (self, **kw)
        self.path = Record (extract_localPathDefs(buildSetup))    # e.g. buildExe -> env.path.buildExe
        self.libInfo = {}
        self.Tool("BuilderGCH")
        self.Tool("BuilderDoxygen")
        self.Tool("ToolDistCC")
        self.Tool("ToolCCache")
        register_LumieraResourceBuilder(self)
        register_LumieraCustomBuilders(self)
    
    
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
    
    
    def addLibInfo (self, libID, minVersion=0, alias=None):
        """ use pkg-config to create an Environment describing the lib.
            Don't add this defs to the current Environment, rather store
            them in the libInfo Dictionary. 
        """
        minVersion = str(minVersion)
        if 0 != os.system('pkg-config --print-errors --exists "%s >= %s"' % (libID,minVersion)):
            print "Problems configuring the Library %s (>= %s)" % (libID,minVersion)
            return False
        
        self.libInfo[libID] = libInfo = Environment()
        libInfo["ENV"]["PKG_CONFIG_PATH"] = os.environ.get("PKG_CONFIG_PATH")
        libInfo.ParseConfig ('pkg-config --cflags --libs '+ libID )
        if alias:
            self.libInfo[alias] = libInfo
        return libInfo





# extending the 'Configure' functionality of SCons,
# especially for library dependency checking
ConfigBase = SCons.SConf.SConfBase



class LumieraConfigContext(ConfigBase):
    """ Extends the SCons Configure context with some convenience methods
    """
    def __init__(self, *args,**kw):
        ConfigBase.__init__(self,*args,**kw)
    
    def CheckPkgConfig (self, libID, minVersion=0, alias=None):
        print "Checking for library configuration: %s " % libID
        # self.Message(self,"Checking for library configuration: %s " % libID)
        return self.env.addLibInfo (libID, minVersion, alias)



###############################################################################
####### Lumiera custom tools and builders #####################################


def register_LumieraResourceBuilder(env):
    """ Registers Custom Builders for generating and installing Icons.
        Additionally you need to build the tool (rsvg-convert.c)
        used to generate png from the svg source using librsvg. 
    """
    
    import IconSvgRenderer as renderer  # load Joel's python script for invoking the rsvg-convert (SVG render)
    renderer.rsvgPath = env.subst("$TARGDIR/rsvg-convert")
    
    def invokeRenderer(target, source, env):
        source = str(source[0])
        targetdir = env.subst(env.path.buildIcon)
        if targetdir.startswith('#'): targetdir = targetdir[1:]
        renderer.main([source,targetdir])
        return 0
        
    def createIconTargets(target,source,env):
        """ parse the SVG to get the target file names """
        source = str(source[0])
        targetdir = env.path.buildIcon
        targetfiles = renderer.getTargetNames(source)    # parse SVG
        
        # additionally create an installation task for each Icon to be generated
        installLocation = env.path.installIcon
        generateTargets = []
        for icon in targetfiles:
            icon = targetdir+icon
            subdir = getDirname(str(icon))
            env.Install (installLocation+subdir, icon)
            generateTargets.append(icon) 
        
        return (generateTargets, source)
    
    def IconResource(env, source):
        """ copy icon pixmap to corresponding icon dir. """
        subdir = getDirname(str(source))
        toBuild = env.path.buildIcon+subdir
        toInstall = env.path.installIcon+subdir
        env.Install (toInstall, source)
        return env.Install(toBuild, source)
    
    def GuiResource(env, source):
        """ pick up giben source resource and install
            them (flat) into the configured target
        """
        toBuild = env.path.buildUIRes
        toInstall = env.path.installUIRes
        env.Install (toInstall, source)
        return env.Install(toBuild, source)
    
    def ConfigData(env, prefix, source, targetDir=None):
        """ install (copy) configuration- and metadata.
            target dir is either the install location configured (in SConstruct),
            or an explicitly given absolute or relative path segment, which might refer
            to the location of the executable through the $ORIGIN token
        """   
        source = path.join(prefix,str(source))
        subdir = getDirname(source, prefix)  # removes source location path prefix
        if targetDir:
            if path.isabs(targetDir):
                toBuild = toInstall = path.join(targetDir,subdir)
            else:
                if targetDir.startswith('$ORIGIN'):
                    targetDir = targetDir[len('$ORIGIN'):]
                    toBuild = path.join(env.path.buildExe, targetDir, subdir)
                    toInstall = path.join(env.path.installExe, targetDir, subdir)
                else:
                    toBuild = path.join(env.path.buildConf, targetDir, subdir)
                    toInstall = path.join(env.path.installConf, targetDir, subdir)
        else:
            toBuild = path.join(env.path.buildConf,subdir)
            toInstall = path.join(env.path.installConf,subdir)
        env.Install (toInstall, source)
        return env.Install(toBuild, source)
    
    
    buildIcon = env.Builder( action = Action(invokeRenderer, "rendering Icon: $SOURCE --> $TARGETS")
                           , single_source = True
                           , emitter = createIconTargets
                           )
    env.Append(BUILDERS = {'IconRender' : buildIcon})
    env.AddMethod(IconResource)
    env.AddMethod(GuiResource)
    env.AddMethod(ConfigData)




class WrappedStandardExeBuilder(SCons.Util.Proxy):
    """ Helper to add customisations and default configurations to SCons standard builders.
        The original builder object is wrapped and most calls are simply forwarded to this
        wrapped object by Python magic. But some calls are intercepted in order to inject
        suitable default configuration based on the project setup.
    """
    
    def __init__(self, originalBuilder):
        SCons.Util.Proxy.__init__ (self, originalBuilder)
    
    def __nonzero__(self): return True
    
    def __call__(self, env, target=None, source=None, **kw):
        """ when the builder gets invoked from the SConscript...
            create a clone environment for specific configuration
            and then pass on the call to the wrapped original builder.
            Automatically define installation targets for build results.
            @note only returning the build targets, not the install targets 
        """
        customisedEnv = self.getCustomEnvironment(env, target=target, **kw)    # defined in subclasses
        buildTarget   = self.buildLocation(customisedEnv, target)
        buildTarget   = self.invokeOriginalBuilder(customisedEnv, buildTarget, source, **kw)
        self.installTarget(customisedEnv, buildTarget, **kw) 
        return buildTarget 
    
    
    def invokeOriginalBuilder(self, env, target, source, **kw):
        return self.get().__call__ (env, target, source, **kw)
    
    def buildLocation(self, env, target):
        """ prefix project output directory """
        prefix = self.getBuildDestination(env)
        return list(prefix+str(name) for name in target)
    
    def installTarget(self, env, buildTarget, **kw):
        """ create an additional installation target
            for the generated executable artifact
        """
        indeedInstall = lambda p: p and p.get('install')
        
        if indeedInstall(kw):
            return env.Install (dir = self.getInstallDestination(env), source=buildTarget)
        else:
            return []




class LumieraExeBuilder(WrappedStandardExeBuilder):
    
    def getCustomEnvironment(self, lumiEnv, **kw):
        """ augments the built-in Program() builder to add a fixed rpath based on $ORIGIN
            That is: after searching LD_LIBRARY_PATH, but before the standard linker search,
            the directory relative to the position of the executable ($ORIGIN) is searched.
            This search path is active not only for the executable, but for all libraries
            it is linked with.
            @note: enabling the new ELF dynamic tags. This causes a DT_RUNPATH to be set,
                   which results in LD_LIBRARY_PATH being searched *before* the RPATH
        """
        custEnv = lumiEnv.Clone()
        custEnv.Append( LINKFLAGS = "-Wl,-rpath=\\$$ORIGIN/modules,--enable-new-dtags" )
        custEnv.Append( LINKFLAGS = "-Wl,-rpath-link=target/modules" ) ### Workaround for bug in binutils > 2.23   /////TICKET #965
        if 'addLibs' in kw:
            custEnv.Append(LIBS = kw['addLibs'])
        return custEnv
    
    def getBuildDestination(self, lumiEnv):   return lumiEnv.path.buildExe
    def getInstallDestination(self, lumiEnv): return lumiEnv.path.installExe
        



class LumieraModuleBuilder(WrappedStandardExeBuilder):
    
    def getCustomEnvironment(self, lumiEnv, target, **kw):
        """ augments the built-in SharedLibrary() builder to add  some tweaks missing in SCons 1.0,
            like setting a SONAME proper instead of just passing the relative pathname to the linker.
            Besides, we override the library search path to allow for transitive dependencies between
            Lumiera modules; modules are assumed to reside in a subdirectory below the executable. 
        """
        custEnv = lumiEnv.Clone()
        custEnv.Append(LINKFLAGS = "-Wl,-soname="+self.defineSoname(target,**kw))
        custEnv.Append( LINKFLAGS = "-Wl,-rpath=\\$$ORIGIN/../modules,--enable-new-dtags" )
        if 'addLibs' in kw:
            custEnv.Append(LIBS = kw['addLibs'])
        return custEnv
    
    def getBuildDestination(self, lumiEnv):   return lumiEnv.path.buildLib
    def getInstallDestination(self, lumiEnv): return lumiEnv.path.installLib
    
    
    def defineSoname (self, target, **kw):
        """ internal helper to extract or guess
            a suitable library SONAME, either using an
            explicit spec, falling back on the lib filename
        """
        if 'soname' in kw:
            soname = self.subst(kw['soname'])  # explicitly defined by user
        else:                                  # else: use the library filename as DT_SONAME
            if SCons.Util.is_String(target):
                pathname = target.strip()
            elif 1 == len(target):
                pathname = str(target[0]).strip()
            else:
                raise SyntaxError("Lumiera Library builder requires exactly one target spec. Found target="+str(target))
            
            assert pathname
            (dirprefix, libname) = path.split(pathname)
            if not libname:
                raise ValueError("Library name missing. Only got a directory: "+pathname)
            
            soname = "${SHLIBPREFIX}%s$SHLIBSUFFIX" % libname
        
        assert soname
        return soname



class LumieraPluginBuilder(LumieraModuleBuilder):
    
    def getCustomEnvironment(self, lumiEnv, target, **kw):
        """ in addition to the ModuleBuilder, define the Lumiera plugin suffix
        """
        custEnv = LumieraModuleBuilder.getCustomEnvironment(self, lumiEnv, target, **kw)
        custEnv.Append (CPPDEFINES='LUMIERA_PLUGIN')
        custEnv.Replace(SHLIBPREFIX='', SHLIBSUFFIX='.lum')
        return custEnv
    
    def getBuildDestination(self, lumiEnv):   return lumiEnv.path.buildPlug
    def getInstallDestination(self, lumiEnv): return lumiEnv.path.installPlug






def register_LumieraCustomBuilders (lumiEnv):
    """ install the customised builder versions tightly integrated with our build system.
        Especially, these builders automatically add the build and installation locations
        and set the RPATH and SONAME in a way to allow a relocatable Lumiera directory structure
    """
    programBuilder = LumieraExeBuilder    (lumiEnv['BUILDERS']['Program'])
    libraryBuilder = LumieraModuleBuilder (lumiEnv['BUILDERS']['SharedLibrary'])
    smoduleBuilder = LumieraModuleBuilder (lumiEnv['BUILDERS']['LoadableModule'])
    lpluginBuilder = LumieraPluginBuilder (lumiEnv['BUILDERS']['LoadableModule'])
    
    lumiEnv['BUILDERS']['Program']        = programBuilder
    lumiEnv['BUILDERS']['SharedLibrary']  = libraryBuilder
    lumiEnv['BUILDERS']['LoadableModule'] = smoduleBuilder
    lumiEnv['BUILDERS']['LumieraPlugin']  = lpluginBuilder
    
    
    def SymLink(env, target, source, linktext=None):
        """ use python to create a symlink
        """
        def makeLink(target,source,env):
            if linktext:
                dest = linktext
            else:
                dest = str(source[0])
            link = str(target[0])
            os.symlink(dest, link)
        
        if linktext: srcSpec=linktext
        else:        srcSpec='$SOURCE'
        action = Action(makeLink, "Install link:  $TARGET -> "+srcSpec)
        env.Command (target,source, action)
    
    # adding SymLink directly as method on the environment object
    # Probably that should better be a real builder, but I couldn't figure out
    # how to get the linktext through literally, which is necessary for relative links.
    # Judging from the sourcecode of SCons.Builder.BuilderBase, there seems to be no way
    # to set the executor_kw, which are passed through to the action object.
    lumiEnv.AddMethod(SymLink)
