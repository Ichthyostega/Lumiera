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

import os
import re

from Buildhelper import *

#-----------------------------------Configuration
OPTIONSCACHEFILE = 'optcache' 
CUSTOPTIONSFILE  = 'custom-options'
SRCDIR           = 'src'
BINDIR           = 'src/bin'
VERSION          = '3.alpha.1'
#-----------------------------------Configuration





#####################################################################

def setupBasicEnvironment():
    ''' define cmdline options, build type decisions
    '''
    opts = defineCmdlineOptions() 

    env = Environment( options=opts
                     , CPPDEFINES={'DEBUG': '${DEBUG}'
                                  ,'USE_OPENGL': '${OPENGL}'
                                  } 
                     )
    env.Replace( VERSION=VERSION
               , SRCDIR=SRCDIR
               , BINDIR=BINDIR
               , CPPPATH=SRCDIR   # used to find includes
               )
    prepareOptionsHelp(opts,env)
    opts.Save(OPTIONSCACHEFILE, env)
    return env


def defineCmdlineOptions():
    ''' current options will be persisted in a options cache file.
        you may define custom options in a separate file. 
        Commandline will override both.
    '''
    opts = Options([OPTIONSCACHEFILE, CUSTOPTIONSFILE])
    opts.AddOptions(
        ('ARCHFLAGS', 'Set architecture-specific compilation flags (passed literally to gcc)','')
        ,BoolOption('DEBUG', 'Build with debugging information and no optimizations', False)
        ,BoolOption('OPTIMIZE', 'Build with strong optimization (-O3)', False)
        ,BoolOption('OPENGL', 'Include support for OpenGL preview rendering', False)
#       ,EnumOption('DIST_TARGET', 'Build target architecture', 'auto', 
#                   allowed_values=('auto', 'i386', 'i686', 'x86_64' ), ignorecase=2)
        ,PathOption('DESTDIR', 'Installation dir prefix', '/usr/local')
        ,PackageOption('TARSRC', 'Create source tarball prior to compiling', '.')
        ,PackageOption('TARDOC', 'Create tarball with dev documentaion, wiki and uml model', '.')
     )

    return opts


def prepareOptionsHelp(opts,env):
    prelude = '''
USAGE:   scons [-c] [OPTS] [TARGETS]
     Build and optionally install Cinelerra.
     Without specifying any target, just the (re)build target will run.
     Add -c to the commandline to clean up anything a given target would produce

Configuration Options:
'''
    Help(prelude + opts.GenerateHelpText(env))



def configurePlatform(env):
    ''' locate required libs.
        setup platform specific options.
        Abort build in case of failure.
    '''
    conf = Configure(env)
    # run all configuration checks in the current env
    
    # Checks for prerequisites ------------
    if not conf.CheckLibWithHeader('m', 'math.h','C'):
        print 'Did not find math.h / libm, exiting.'
        Exit(1)
    
    if not conf.CheckCXXHeader('boost/config.hpp'):
        print 'We need the C++ boost-lib.'
        Exit(1)
    
    if not conf.CheckCXXHeader('boost/shared_ptr.hpp'):
        print 'We need boost::shared_ptr (shared_ptr.hpp).'
        Exit(1)
        
    # create new env containing the finished configuration
    return conf.Finish()



def definePackagingTargets(env, artifacts):
    ''' build operations and targets to be done /before/ compiling.
        things like creating a source tarball or preparing a version header.
    '''
    env.SetDefault(TARFLAGS = '-c -z', TARSUFFIX = '')
    if env['TARSRC']:
        # define the Tar as a target and make it default,
        # i.e. buid it if scons is called without targets
        t=env.Tar(getTarName(env['TARSRC']),SRCDIR)
        env.Default(t)
    if env['TARDOC']:
        t=env.Tar(getTarName(env['TARDOC']), 'admin doc wiki uml tests')
        env.Default(t)

def getTarName(spec):
    (head,tail) = os.path.split( os.path.abspath(spec))
    if not os.path.isdir(head):
        print 'Target dir "%s" for Tar doesn\'t exist.' % head
        Exit(1)
    mat = re.match(r'([\w\.\-])\.((tar)|(tar\.gz)|(tgz))', tail)
    if mat:
        name = mat.group(1)
        ext  = '.'+mat.group(2)
    else:
        ext = '.tar.gz'
        if os.path.isdir(tail):
            name = 'cinelerra$VERSION'
        else:
            name = tail
    return os.path.join(head,name+ext)
            


def defineBuildTargets(env, artifacts):
    ''' define the source file/dirs comprising each artifact to be built.
        setup sub-environments with special build options if necessary 
    '''
    cinobj = ( srcSubtree(env,'backend') 
             + srcSubtree(env,'proc')
             + env.Object('$SRCDIR/main.cpp')
             )
    plugobj = srcSubtree(env,'plugin', isShared=True)
    
    artifacts['cinelerra'] = env.Program('$BINDIR/cinelerra', cinobj)
    artifacts['plugins']   = env.SharedLibrary('$BINDIR/cinelerra-plugin', plugobj)
    
    # call subdir SConscript(s)
    SConscript(dirs=[SRCDIR+'/tool'], exports='env artifacts')
    


def defineInstallTargets(env, artifacts):
    ''' define install locations and cleanup after the build.
        define alias targets to trigger the installing.
    '''
    env.Install(dir = '$DESTDIR/bin', source=artifacts['cinelerra'])
    env.Install(dir = '$DESTDIR/lib', source=artifacts['plugins'])
    env.Install(dir = '$DESTDIR/bin', source=artifacts['tools'])
    
    ib = env.Alias('install-bin', '$DESTDIR/bin')
    il = env.Alias('install-lib', '$DESTDIR/lib')
    env.Alias('install', [ib, il])
    
    env.Alias('build', '$DESTDIR')
    env.Default('build')
    # additional files to be cleaned when cleaning 'build'
    env.Clean ('build', [ 'scache.conf', '.sconf_temp', '.sconsign.dblite', 'config.log'])

#####################################################################





### === MAIN === ####################################################

# NOTE: the following code /defines/ what and how to build
#       it doesn't "do" the build. SCons will do the "doing"

env = setupBasicEnvironment()

if not (isCleanupOperation(env) or isHelpRequest()):
    env = configurePlatform(env)
    
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

