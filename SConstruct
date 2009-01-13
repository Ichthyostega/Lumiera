# -*- python -*-
##
## SConstruct  -  SCons based build-sytem for Lumiera
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


#-----------------------------------Configuration
OPTIONSCACHEFILE = 'optcache' 
CUSTOPTIONSFILE  = 'custom-options'
SRCDIR           = 'src'
BINDIR           = 'bin'
LIBDIR           = '.libs'
PLUGDIR          = '.libs'
TESTDIR          = 'tests'
ICONDIR          = 'icons'
VERSION          = '0.1+pre.01'
TOOLDIR          = './admin/scons'
SVGRENDERER      = 'admin/render-icon'
#-----------------------------------Configuration

# NOTE: scons -h for help.
# Read more about the SCons build system at: http://www.scons.org
# Basically, this script just /defines/ the components and how they
# fit together. SCons will derive the necessary build steps.



import os
import sys

sys.path.append(TOOLDIR)

from Buildhelper import *
from LumieraEnvironment import *


#####################################################################

def setupBasicEnvironment():
    """ define cmdline options, build type decisions
    """
    EnsurePythonVersion(2,3)
    EnsureSConsVersion(0,96,90)
    
    opts = defineCmdlineOptions() 
    env = LumieraEnvironment(options=opts
                            ,toolpath = [TOOLDIR]
                            ,tools = ["default", "BuilderGCH", "BuilderDoxygen"]  
                            ) 
    env.Tool("ToolDistCC")
    env.Tool("ToolCCache")
    handleVerboseMessages(env)
    
    env.Append ( CCCOM=' -std=gnu99') 
    env.Append ( SHCCCOM=' -std=gnu99') # workaround for a bug: CCCOM currently doesn't honour CFLAGS, only CCFLAGS 
    env.Replace( VERSION=VERSION
               , SRCDIR=SRCDIR
               , BINDIR=BINDIR
               , LIBDIR=LIBDIR
               , PLUGDIR=PLUGDIR
               , ICONDIR=ICONDIR
               , CPPPATH=["#"+SRCDIR]   # used to find includes, "#" means always absolute to build-root
               , CPPDEFINES=['-DLUMIERA_VERSION='+VERSION ]     # note: it's a list to append further defines
               , CCFLAGS='-Wall -Wextra '
               , CFLAGS='-std=gnu99' 
               )
    RegisterIcon_Builder(env,SVGRENDERER)
    handleNoBugSwitches(env)
    
    env.Append(CPPDEFINES = '_GNU_SOURCE')
    appendCppDefine(env,'DEBUG','DEBUG', 'NDEBUG')
#   appendCppDefine(env,'OPENGL','USE_OPENGL')
    appendVal(env,'ARCHFLAGS','CCFLAGS')   # for both C and C++
    appendVal(env,'OPTIMIZE', 'CCFLAGS',   val=' -O3')
    appendVal(env,'DEBUG',    'CCFLAGS',   val=' -ggdb')
    
    # setup search path for Lumiera plugins
    appendCppDefine(env,'PKGLIBDIR','LUMIERA_PLUGIN_PATH=\\"$PKGLIBDIR\\"'
                                   ,'LUMIERA_PLUGIN_PATH=\\"$DESTDIR/lib/lumiera\\"') 
    appendCppDefine(env,'PKGDATADIR','LUMIERA_CONFIG_PATH=\\"$PKGLIBDIR\\"'
                                    ,'LUMIERA_CONFIG_PATH=\\"$DESTDIR/share/lumiera\\"') 
    
    prepareOptionsHelp(opts,env)
    opts.Save(OPTIONSCACHEFILE, env)
    return env

def appendCppDefine(env,var,cppVar, elseVal=''):
    if env[var]:
        env.Append(CPPDEFINES = env.subst(cppVar) )
    elif elseVal:
        env.Append(CPPDEFINES = env.subst(elseVal))

def appendVal(env,var,targetVar,val=None):
    if env[var]:
        env.Append( **{targetVar: env.subst(val) or env[var]})


def handleNoBugSwitches(env):
    """ set the build level for NoBug. 
        Release builds imply no DEBUG
        whereas ALPHA and BETA require DEBUG
    """
    level = env['BUILDLEVEL']
    if level in ['ALPHA', 'BETA']:
        if not env['DEBUG']:
            print 'Warning: NoBug ALPHA or BETA builds requires DEBUG=yes, switching DEBUG on!'
        env.Replace( DEBUG = 1 )
        env.Append(CPPDEFINES = 'EBUG_'+level)
    elif level == 'RELEASE':
        env.Replace( DEBUG = 0 )

def handleVerboseMessages(env):
    """ toggle verbose build output """
    if not env['VERBOSE']:
       # SetOption('silent', True)
       env['CCCOMSTR'] = env['SHCCCOMSTR']   = "  Compiling    $SOURCE"
       env['CXXCOMSTR'] = env['SHCXXCOMSTR'] = "  Compiling++  $SOURCE"
       env['LINKCOMSTR']                     = "  Linking -->  $TARGET"
       env['LDMODULECOMSTR']                 = "  creating module [ $TARGET ]"




def defineCmdlineOptions():
    """ current options will be persisted in a options cache file.
        you may define custom options in a separate file. 
        Commandline will override both.
    """
    opts = Options([OPTIONSCACHEFILE, CUSTOPTIONSFILE])
    opts.AddOptions(
         ('ARCHFLAGS', 'Set architecture-specific compilation flags (passed literally to gcc)','')
        ,('CC', 'Set the C compiler to use.', 'gcc')
        ,('CXX', 'Set the C++ compiler to use.', 'g++')
        ,PathOption('CCACHE', 'Integrate with CCache', '', PathOption.PathAccept)
        ,PathOption('DISTCC', 'Invoke C/C++ compiler commands through DistCC', '', PathOption.PathAccept)
        ,EnumOption('BUILDLEVEL', 'NoBug build level for debugging', 'ALPHA',
                    allowed_values=('ALPHA', 'BETA', 'RELEASE'))
        ,BoolOption('DEBUG', 'Build with debugging information and no optimisations', False)
        ,BoolOption('OPTIMIZE', 'Build with strong optimisation (-O3)', False)
        ,BoolOption('VALGRIND', 'Run Testsuite under valgrind control', True)
        ,BoolOption('VERBOSE',  'Print full build commands', False)
        ,('TESTSUITES', 'Run only Testsuites matching the given pattern', '')
#       ,BoolOption('OPENGL', 'Include support for OpenGL preview rendering', False)
#       ,EnumOption('DIST_TARGET', 'Build target architecture', 'auto', 
#                   allowed_values=('auto', 'i386', 'i686', 'x86_64' ), ignorecase=2)
        ,PathOption('DESTDIR', 'Installation dir prefix', '/usr/local')
        ,PathOption('PKGLIBDIR', 'Installation dir for plugins, defaults to DESTDIR/lib/lumiera', '',PathOption.PathAccept)
        ,PathOption('PKGDATADIR', 'Installation dir for default config, usually DESTDIR/share/lumiera', '',PathOption.PathAccept)
        ,PathOption('SRCTAR', 'Create source tarball prior to compiling', '..', PathOption.PathAccept)
        ,PathOption('DOCTAR', 'Create tarball with developer documentation', '..', PathOption.PathAccept)
     )
    
    return opts



def prepareOptionsHelp(opts,env):
    prelude = """
USAGE:   scons [-c] [OPTS] [key=val [key=val...]] [TARGETS]
     Build and optionally install Lumiera.
     Without specifying any target, just the (re)build target will run.
     Add -c to the commandline to clean up anything a given target would produce

Special Targets:
     build   : just compile and link
     testcode: additionally compile the Testsuite
     check   : build and run the Testsuite
     doc     : generate documentation (Doxygen)
     install : install created artifacts at PREFIX
     src.tar : create source tarball
     doc.tar : create developer doc tarball
     tar     : create all tarballs

Configuration Options:
"""
    Help(prelude + opts.GenerateHelpText(env))




def configurePlatform(env):
    """ locate required libs.
        setup platform specific options.
        Abort build in case of failure.
    """
    conf = env.Configure()
    # run all configuration checks in the given env
    
    # Perform checks for prerequisites --------------------------------------------
    problems = []
    if not conf.TryAction('pkg-config --version > $TARGET')[0]:
        problems.append('We need pkg-config for including library configurations, exiting.')
    
    if not conf.CheckLibWithHeader('m', 'math.h','C'):
        problems.append('Did not find math.h / libm.')
    
    if not conf.CheckLibWithHeader('dl', 'dlfcn.h', 'C'):
        problems.append('Functions for runtime dynamic loading not available.')
    
    if not conf.CheckPkgConfig('nobugmt', 0.3):
        problems.append('Did not find NoBug [http://www.pipapo.org/pipawiki/NoBug].')
    else:
        conf.env.mergeConf('nobugmt')
    
    if not conf.CheckLibWithHeader('pthread', 'pthread.h', 'C'):
        problems.append('Did not find the pthread lib or pthread.h.')
    else:
       conf.env.Append(CPPFLAGS = ' -DHAVE_PTHREAD')
       conf.env.Append(CCFLAGS = ' -pthread')
    
    if conf.CheckCHeader('execinfo.h'):
       conf.env.Append(CPPFLAGS = ' -DHAS_EXECINFO_H')
    
    if conf.CheckCHeader('valgrind/valgrind.h'):
        conf.env.Append(CPPFLAGS = ' -DHAS_VALGRIND_VALGIND_H')
    
    if not conf.CheckCXXHeader('tr1/memory'):
        problems.append('We rely on the std::tr1 proposed standard extension for shared_ptr.')
    
    if not conf.CheckCXXHeader('boost/config.hpp'):
        problems.append('We need the C++ boost-lib.')
    else:
        if not conf.CheckCXXHeader('boost/shared_ptr.hpp'):
            problems.append('We need boost::shared_ptr (shared_ptr.hpp).')
        if not conf.CheckLibWithHeader('boost_program_options-mt','boost/program_options.hpp','C++'):
            problems.append('We need boost::program_options (including binary lib for linking).')
        if not conf.CheckLibWithHeader('boost_regex-mt','boost/regex.hpp','C++'):
            problems.append('We need the boost regular expression lib (incl. binary lib for linking).')
    
    
    if not conf.CheckPkgConfig('gavl', 1.0):
        problems.append('Did not find Gmerlin Audio Video Lib [http://gmerlin.sourceforge.net/gavl.html].')
    else:
        conf.env.mergeConf('gavl')
    
    if not conf.CheckPkgConfig('gtkmm-2.4', 2.8):
        problems.append('Unable to configure GTK--, exiting.')
        
    if not conf.CheckPkgConfig('glibmm-2.4', '2.16'):
        problems.append('Unable to configure Lib glib--, exiting.')
    
    if not conf.CheckPkgConfig('gthread-2.0', '2.16'):
        problems.append('Need gthread support lib for glib-- based thread handling.')
    
    if not conf.CheckPkgConfig('cairomm-1.0', 0.6):
        problems.append('Unable to configure Cairo--, exiting.')
    
    if not conf.CheckPkgConfig('gdl-1.0', '0.6.1'):
        problems.append('Unable to configure the GNOME DevTool Library.')
    
    if not conf.CheckPkgConfig('librsvg-2.0', '2.18.1'):
        problems.append('Need rsvg Library for rendering icons.')
        
    if not conf.CheckCHeader(['X11/Xutil.h', 'X11/Xlib.h'],'<>'):
        problems.append('Xlib.h and Xutil.h required. Please install libx11-dev.')
    
    if not conf.CheckPkgConfig('xv')  : problems.append('Need libXv...')
    if not conf.CheckPkgConfig('xext'): problems.append('Need libXext.')
#   if not conf.CheckPkgConfig('sm'): Exit(1)
#    
# obviously not needed?
    
    
    # report missing dependencies
    if problems:
        print "*** unable to build due to the following problems:"
        for isue in problems:
            print " *  %s" % isue
        print
        print "build aborted."
        Exit(1)
    
    print "** Gathered Library Info: %s" % conf.env.libInfo.keys()
    
    
    # create new env containing the finished configuration
    return conf.Finish()



def definePackagingTargets(env, artifacts):
    """ build operations and targets to be done /before/ compiling.
        things like creating a source tarball or preparing a version header.
    """
    t = Tarball(env,location='$SRCTAR',dirs='$SRCDIR')
    artifacts['src.tar'] = t
    env.Alias('src.tar', t)
    env.Alias('tar', t)
    
    t =  Tarball(env,location='$DOCTAR',suffix='-doc',dirs='admin doc wiki uml tests')
    artifacts['doc.tar'] = t
    env.Alias('doc.tar', t)
    env.Alias('tar', t)



def defineBuildTargets(env, artifacts):
    """ define the source file/dirs comprising each artifact to be built.
        setup sub-environments with special build options if necessary.
        We use a custom function to declare a whole tree of srcfiles. 
    """
    env.mergeConf(['glibmm-2.4','gthread-2.0'])
    
    # use PCH to speed up building
#   env['GCH'] = ( env.PrecompiledHeader('$SRCDIR/pre.hpp')
#                + env.PrecompiledHeader('$SRCDIR/pre_a.hpp')
#                )
    
    
    
    lApp  = env.SharedLibrary('$LIBDIR/lumieracommon',  srcSubtree(env,'$SRCDIR/common'))
    lBack = env.SharedLibrary('$LIBDIR/lumierabackend', srcSubtree(env,'$SRCDIR/backend'))
    lProc = env.SharedLibrary('$LIBDIR/lumieraproc',    srcSubtree(env,'$SRCDIR/proc'))
    lLib  = env.SharedLibrary('$LIBDIR/lumiera',        srcSubtree(env,'$SRCDIR/lib'))
    
    core = lLib+lApp+lBack+lProc
    
    artifacts['lumiera'] = env.Program('$BINDIR/lumiera', ['$SRCDIR/lumiera/main.cpp'], LIBS=core)
    artifacts['corelib'] = lLib+lApp
    artifacts['support'] = lLib
    
    # building Lumiera Plugins
    envPlu = env.Clone()
    envPlu.Append(CPPDEFINES='LUMIERA_PLUGIN')
    artifacts['plugins'] = [] # currently none 
    
    
    # the Lumiera GTK GUI
    envGtk = env.Clone()
    envGtk.mergeConf(['gtkmm-2.4','cairomm-1.0','gdl-1.0','librsvg-2.0','xv','xext','sm'])
    envGtk.Append(CPPDEFINES='LUMIERA_PLUGIN', LIBS=core)
    objgui  = srcSubtree(envGtk,'$SRCDIR/gui')
    
    # render and install Icons
    vector_icon_dir      = env.subst('$ICONDIR/svg')
    prerendered_icon_dir = env.subst('$ICONDIR/prerendered')
    artifacts['icons']   = ( [env.IconRender(f) for f in scanSubtree(vector_icon_dir,      ['*.svg'])]
                           + [env.IconCopy(f)   for f in scanSubtree(prerendered_icon_dir, ['*.png'])]
                           )
    
    guimodule = envGtk.LoadableModule('$LIBDIR/gtk_gui', objgui, SHLIBPREFIX='', SHLIBSUFFIX='.lum')
    artifacts['lumigui'] = ( guimodule
                           + envGtk.Program('$BINDIR/lumigui', objgui )
                           + env.Install('$BINDIR', env.Glob('$SRCDIR/gui/*.rc'))
                           + artifacts['icons']
                           )
    
    # call subdir SConscript(s) for independent components
    SConscript(dirs=[SRCDIR+'/tool'], exports='env envGtk artifacts core')
    SConscript(dirs=[TESTDIR],        exports='env envPlu artifacts core')



def definePostBuildTargets(env, artifacts):
    """ define further actions after the core build (e.g. Documentaion).
        define alias targets to trigger the installing.
    """
    ib = env.Alias('install-bin', '$DESTDIR/bin')
    il = env.Alias('install-lib', '$DESTDIR/lib')
    env.Alias('install', [ib, il])
    
    build = env.Alias('build', artifacts['lumiera']+artifacts['lumigui']+artifacts['plugins']+artifacts['tools'])
    allbu = env.Alias('allbuild', build+artifacts['testsuite'])
    env.Default('build')
    # additional files to be cleaned when cleaning 'build'
    env.Clean ('build', [ 'scache.conf', '.sconf_temp', '.sconsign.dblite', 'config.log' ])
    env.Clean ('build', [ '$SRCDIR/pre.gch' ])
    
    doxydoc = artifacts['doxydoc'] = env.Doxygen('doc/devel/Doxyfile')
    env.Alias ('doc', doxydoc)
    env.Clean ('doc', doxydoc + ['doc/devel/,doxylog','doc/devel/warnings.txt'])


def defineInstallTargets(env, artifacts):
    """ define some artifacts to be installed into target locations.
    """
    env.Install(dir = '$DESTDIR/bin', source=artifacts['lumiera'])
    env.Install(dir = '$DESTDIR/lib', source=artifacts['corelib'])
    env.Install(dir = '$DESTDIR/lib', source=artifacts['plugins'])
    env.Install(dir = '$DESTDIR/bin', source=artifacts['tools'])
    
    env.Install(dir = '$DESTDIR/share/doc/lumiera$VERSION/devel', source=artifacts['doxydoc'])

#####################################################################





### === MAIN === ####################################################


env = setupBasicEnvironment()

if not (isCleanupOperation(env) or isHelpRequest()):
    env = configurePlatform(env)
    
artifacts = {}
# the various things we build. 
# Each entry actually is a SCons-Node list.
# Passing these entries to other builders defines dependencies.
# 'lumiera'     : the App
# 'plugins'     : plugin shared lib
# 'tools'       : small tool applications (e.g mpegtoc)
# 'src,tar'     : source tree as tarball (without doc)
# 'doc.tar'     : uml model, wiki, dev docu (no src)

definePackagingTargets(env, artifacts)
defineBuildTargets(env, artifacts)
definePostBuildTargets(env, artifacts)
defineInstallTargets(env, artifacts)

