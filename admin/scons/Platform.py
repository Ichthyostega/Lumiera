# -*- python -*-
##
## Platform.py  -  SCons build: platform configuration and library detection
##

#  Copyright (C)         Lumiera.org
#    2012,               Hermann Vosseler <Ichthyostega@web.de>
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
# Read more about the SCons build system at: http://www.scons.org
# Basically, this script just /defines/ the components and how they
# fit together. SCons will derive the necessary build steps.


#-----------------------------------Configuration
TARGDIR      = 'target'
VERSION      = '0.pre.01'
TOOLDIR      = './admin/scons'    # SCons plugins
SCRIPTDIR    = './admin'
OPTCACHE     = 'optcache' 
CUSTOPTFILE  = 'custom-options'

# these are accessible via env.path.xxxx
srcIcon      = 'icons'
srcConf      = 'data/config'
buildExe     = '#$TARGDIR'
buildLib     = '#$TARGDIR/modules'
buildPlug    = '#$TARGDIR/modules'
buildIcon    = '#$TARGDIR/gui/icons'
buildUIRes   = '#$TARGDIR/'
buildConf    = '#$TARGDIR/config'
installExe   = '#$DESTDIR/lib/lumiera'
installLib   = '#$DESTDIR/lib/lumiera/modules'
installPlug  = '#$DESTDIR/lib/lumiera/modules'
installIcon  = '#$DESTDIR/share/lumiera/icons'
installUIRes = '#$DESTDIR/share/lumiera/'
installConf  = '#$DESTDIR/lib/lumiera/config'

#-----------------------------------Configuration
localDefinitions = locals()




import os
import sys

sys.path.append(TOOLDIR)
sys.path.append(SCRIPTDIR)

from Buildhelper import *
from LumieraEnvironment import *


#####################################################################

def setupBasicEnvironment(localDefinitions):
    """ define cmdline options, build type decisions
    """
    EnsurePythonVersion(2,4)
    EnsureSConsVersion(1,0)
    
    Decider('MD5-timestamp') # detect changed files by timestamp, then do a MD5
    
    vars = defineCmdlineVariables() 
    env = LumieraEnvironment(variables=vars
                            ,toolpath = [TOOLDIR]
                            ,pathConfig = extract_localPathDefs(localDefinitions) # e.g. buildExe -> env.path.buildExe
                            ,TARGDIR  = TARGDIR
                            ,DESTDIR = '$INSTALLDIR/$PREFIX'
                            ,VERSION = VERSION
                            )
    handleVerboseMessages(env)
    
    env.Replace( CPPPATH   =["#src"]    # used to find includes, "#" means always absolute to build-root
               , CPPDEFINES=['LUMIERA_VERSION='+VERSION ]    # note: it's a list to append further defines
               , CCFLAGS='-Wall -Wextra '
               , CFLAGS='-std=gnu99' 
               )
    handleNoBugSwitches(env)
    
    env.Append(CPPDEFINES = '_GNU_SOURCE')
    appendCppDefine(env,'DEBUG','DEBUG', 'NDEBUG')
#   appendCppDefine(env,'OPENGL','USE_OPENGL')
    appendVal(env,'ARCHFLAGS','CCFLAGS')   # for both C and C++
    appendVal(env,'OPTIMIZE', 'CCFLAGS',   val=' -O3')
    appendVal(env,'DEBUG',    'CCFLAGS',   val=' -ggdb')
    
    # setup search path for Lumiera plugins
    appendCppDefine(env,'PKGLIBDIR','LUMIERA_PLUGIN_PATH=\\"$PKGLIBDIR/:ORIGIN/modules\\"'
                                   ,'LUMIERA_PLUGIN_PATH=\\"ORIGIN/modules\\"') 
    appendCppDefine(env,'PKGDATADIR','LUMIERA_CONFIG_PATH=\\"$PKGLIBDIR/:.\\"'
                                    ,'LUMIERA_CONFIG_PATH=\\"$DESTDIR/share/lumiera/:.\\"') 
    
    prepareOptionsHelp(vars,env)
    vars.Save(OPTCACHE, env)
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




def defineCmdlineVariables():
    """ several toggles and configuration variables can be set on the commandline,
        current settings will be persisted in a options cache file.
        you may define custom variable settings in a separate file. 
        Commandline will override both.
    """
    vars = Variables([OPTCACHE, CUSTOPTFILE])
    vars.AddVariables(
         ('ARCHFLAGS', 'Set architecture-specific compilation flags (passed literally to gcc)','')
        ,('CC', 'Set the C compiler to use.', 'gcc')
        ,('CXX', 'Set the C++ compiler to use.', 'g++')
        ,PathVariable('CCACHE', 'Integrate with CCache', '', PathVariable.PathAccept)
        ,PathVariable('DISTCC', 'Invoke C/C++ compiler commands through DistCC', '', PathVariable.PathAccept)
        ,EnumVariable('BUILDLEVEL', 'NoBug build level for debugging', 'ALPHA', allowed_values=('ALPHA', 'BETA', 'RELEASE'))
        ,BoolVariable('DEBUG', 'Build with debugging information and no optimisations', False)
        ,BoolVariable('OPTIMIZE', 'Build with strong optimisation (-O3)', False)
        ,BoolVariable('VALGRIND', 'Run Testsuite under valgrind control', True)
        ,BoolVariable('VERBOSE',  'Print full build commands', False)
        ,('TESTSUITES', 'Run only Testsuites matching the given pattern', '')
#       ,BoolVariable('OPENGL', 'Include support for OpenGL preview rendering', False)
#       ,EnumVariable('DIST_TARGET', 'Build target architecture', 'auto', 
#                   allowed_values=('auto', 'i386', 'i686', 'x86_64' ), ignorecase=2)
        ,PathVariable('PREFIX', 'Installation dir prefix', 'usr/local', PathVariable.PathAccept)
        ,PathVariable('INSTALLDIR', 'Root output directory for install. Final installation will happen in INSTALLDIR/PREFIX/... ', '/', PathVariable.PathIsDir)
        ,PathVariable('PKGLIBDIR', 'Installation dir for plugins, defaults to PREFIX/lib/lumiera/modules', '',PathVariable.PathAccept)
        ,PathVariable('PKGDATADIR', 'Installation dir for default config, usually PREFIX/share/lumiera', '',PathVariable.PathAccept)
     )
    
    return vars



def prepareOptionsHelp(vars,env):
    prelude = """
USAGE:   scons [-c] [OPTS] [key=val [key=val...]] [TARGETS]
     Build and optionally install Lumiera.
     Without specifying any target, just the (re)build target will run.
     Add -c to the commandline to clean up anything a given target would produce

Special Targets:
     build   : just compile and link
     research: build experimental code (might fail)
     testcode: additionally compile the Testsuite
     check   : build and run the Testsuite
     doc     : generate documentation (Doxygen)
     all     : build and testcode and doc
     install : install created artifacts at PREFIX

Configuration Options:
"""
    Help(prelude + vars.GenerateHelpText(env))




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
    
    if not conf.CheckLibWithHeader('pthread', 'pthread.h', 'C'):
        problems.append('Did not find the pthread lib or pthread.h.')
    else:
       conf.env.Append(CPPFLAGS = ' -DHAVE_PTHREAD')
       conf.env.Append(CCFLAGS = ' -pthread')
    
    if conf.CheckCHeader('execinfo.h'):
       conf.env.Append(CPPFLAGS = ' -DHAVE_EXECINFO_H')
    
    if conf.CheckCHeader('valgrind/valgrind.h'):
        conf.env.Append(CPPFLAGS = ' -DHAVE_VALGRIND_H')
    else:
        print 'Valgrind not found. The use of Valgrind is optional; building without.'
    
    if not conf.CheckPkgConfig('nobugmt', 201006.1):
        problems.append('Did not find NoBug [http://www.lumiera.org/nobug_manual.html].')
    else:
        conf.env.mergeConf('nobugmt')
    
    if not conf.CheckCXXHeader('tr1/memory'):
        problems.append('We rely on the std::tr1 standard C++ extension for shared_ptr.')
    
    if not conf.CheckCXXHeader('boost/config.hpp'):
        problems.append('We need the C++ boost-libraries.')
    else:
        if not conf.CheckCXXHeader('boost/scoped_ptr.hpp'):
            problems.append('We need boost::scoped_ptr (scoped_ptr.hpp).')
        if not conf.CheckCXXHeader('boost/format.hpp'):
            problems.append('We need boost::format (header).')
        if not conf.CheckLibWithHeader('boost_program_options-mt','boost/program_options.hpp','C++'):
            problems.append('We need boost::program_options (including binary lib for linking).')
        if not conf.CheckLibWithHeader('boost_system-mt','boost/system/error_code.hpp','C++'):
            problems.append('We need the boost::system support library (including binary lib).')
        if not conf.CheckLibWithHeader('boost_filesystem-mt','boost/filesystem.hpp','C++'):
            problems.append('We need the boost::filesystem lib (including binary lib for linking).')
        if not conf.CheckLibWithHeader('boost_regex-mt','boost/regex.hpp','C++'):
            problems.append('We need the boost regular expression lib (incl. binary lib for linking).')
    
    
    if conf.CheckLib(symbol='clock_gettime'):
        print 'Using function clock_gettime() as defined in the C-lib...'
    else:
        if not conf.CheckLib(symbol='clock_gettime', library='rt'):
            problems.append('No library known to provide the clock_gettime() function.')
    
    if not conf.CheckPkgConfig('gavl', 1.0):
        problems.append('Did not find Gmerlin Audio Video Lib [http://gmerlin.sourceforge.net/gavl.html].')
    else:
        conf.env.mergeConf('gavl')
    
    if not conf.CheckPkgConfig('gtkmm-2.4', 2.8):
        problems.append('Unable to configure GTK--')
        
    if not conf.CheckPkgConfig('glibmm-2.4', '2.16'):
        problems.append('Unable to configure Lib glib--')
    
    if not conf.CheckPkgConfig('gthread-2.0', '2.12.4'):
        problems.append('Need gthread support lib for glib-- based thread handling.')
    
    if not conf.CheckPkgConfig('cairomm-1.0', 0.6):
        problems.append('Unable to configure Cairo--')
    
    verGDL = '2.27.1'
    if not conf.CheckPkgConfig('gdl-1.0', verGDL, alias='gdl'):
        print 'No sufficiently recent (>=%s) version of GDL found. Maybe use custom package gdl-lum?' % verGDL
        if not conf.CheckPkgConfig('gdl-lum', verGDL, alias='gdl'):
            problems.append('GNOME Docking Library not found. We either need a sufficiently recent GDL '
                            'version (>=%s), or the custom package "gdl-lum" from Lumiera.org.' % verGDL)
    
    if not conf.CheckPkgConfig('librsvg-2.0', '2.18.1'):
        problems.append('Need rsvg Library for rendering icons.')
        
    if not conf.CheckCHeader(['X11/Xutil.h', 'X11/Xlib.h'],'<>'):
        problems.append('Xlib.h and Xutil.h required. Please install libx11-dev.')
    
    if not conf.CheckPkgConfig('xv')  : problems.append('Need libXv...')
    if not conf.CheckPkgConfig('xext'): problems.append('Need libXext.')
    
    
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



def defineSetupTargets(env):
    """ build operations and targets to be done /before/ compiling.
        things like creating a source tarball or preparing a version header.
    """
    pass    ## currently none



def defineBuildTargets(env):
    """ define the source file/dirs comprising each artifact to be built.
        setup sub-environments with special build options if necessary.
        We use a custom function to declare a whole tree of srcfiles. 
    """
    
    # define Icons to render and install
    vector_icon_dir      = env.subst(env.path.srcIcon+'svg')
    prerendered_icon_dir = env.subst(env.path.srcIcon+'prerendered')
    icons   = ( [env.IconRender(f)   for f in scanSubtree(vector_icon_dir,      ['*.svg'])]
              + [env.IconResource(f) for f in scanSubtree(prerendered_icon_dir, ['*.png'])]
              )
    
    #define Configuration files to install
    config  = ( env.ConfigData(env.path.srcConf+'setup.ini', targetDir='$ORIGIN')
              + env.ConfigData(env.path.srcConf+'dummy_lumiera.ini')
              )
    
    # call subdir SConscript(s) for independent components
    SConscript(dirs=['src','src/tool','research','tests'], exports='env icons config')



def definePostBuildTargets(env):
    """ define further actions after the core build (e.g. Documentation).
        define alias targets to trigger the installing.
    """
    Import('lumiera plugins tools gui testsuite')
    build = env.Alias('build', lumiera + plugins + tools +gui)
    
    # additional files to be cleaned when cleaning 'build'
    env.Clean ('build', [ 'scache.conf', '.sconf_temp', '.sconsign.dblite', 'config.log' ])
    env.Clean ('build', [ 'src/pre.gch' ])
    
    doxydoc = env.Doxygen('doc/devel/Doxyfile')
    env.Alias ('doc', doxydoc)
    env.Clean ('doc', doxydoc + ['doc/devel/,doxylog','doc/devel/warnings.txt'])
    
    env.Alias ('all', build + testsuite + doxydoc)
    env.Default('build')
    # SCons default target


def defineInstallTargets(env):
    """ define additional artifacts to be installed into target locations.
        @note: we use customised SCons builders defining install targets 
               for all executables automatically. see LumieraEnvironment.py
    """
    Import('lumiera plugins tools gui testsuite')
    
    env.SymLink('$DESTDIR/bin/lumiera',env.path.installExe+'lumiera','../lib/lumiera/lumiera')
#   env.Install(dir = '$DESTDIR/share/doc/lumiera$VERSION/devel', source=doxydoc)
    
    env.Alias('install', gui)
    env.Alias('install', '$DESTDIR')

#####################################################################





### === MAIN === ####################################################

env = setupBasicEnvironment(localDefinitions)

if not (isCleanupOperation(env) or isHelpRequest()):
    env = configurePlatform(env)
    
# the various things we build. 
# Each entry actually is a SCons-Node list.
# Passing these entries to other builders defines dependencies.
# 'lumiera'     : the App
# 'gui'         : the GTK UI (plugin)
# 'plugins'     : plugin shared lib
# 'tools'       : small tool applications (e.g mpegtoc)

defineSetupTargets(env)
defineBuildTargets(env)
definePostBuildTargets(env)
defineInstallTargets(env)

