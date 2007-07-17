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

import sys
sys.path.append("./admin/scons")

import os
from Buildhelper import *

#-----------------------------------Configuration
OPTIONSCACHEFILE = 'optcache' 
CUSTOPTIONSFILE  = 'custom-options'
SRCDIR           = 'src'
BINDIR           = 'src/bin'
VERSION          = '3+alpha.01'
#-----------------------------------Configuration

# NOTE: scons -h for help.
# Read more about the SCons build system at: http://www.scons.org
# Basically, this script just /defines/ the components and how they
# fit together. SCons will derive the necessary build steps.




#####################################################################

def setupBasicEnvironment():
    ''' define cmdline options, build type decisions
    '''
    EnsurePythonVersion(2,3)
    EnsureSConsVersion(0,96,90)
    
    opts = defineCmdlineOptions() 
 
    env = Environment(options=opts) 
    env.Replace( VERSION=VERSION
               , SRCDIR=SRCDIR
               , BINDIR=BINDIR
               , CPPPATH=SRCDIR   # used to find includes
               )
    appendCppDefine(env,'DEBUG','DEBUG')
    appendCppDefine(env,'OPENGL','USE_OPENGL')
    appendVal(env,'ARCHFLAGS', 'CPPFLAGS')   # for both C and C++
    appendVal(env,'OPTIMIZE', 'CPPFLAGS', val=' -O3')
    
    prepareOptionsHelp(opts,env)
    opts.Save(OPTIONSCACHEFILE, env)
    return env

def appendCppDefine(env,var,cppVar):
    if env[var]:
        env.Append(CPPDEFINES = {cppVar: env[var]})

def appendVal(env,var,targetVar,val=None):
    if env[var]:
        env.Append( **{targetVar: val or env[var]})



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
        ,PathOption('SRCTAR', 'Create source tarball prior to compiling', '..', PathOption.PathAccept)
        ,PathOption('DOCTAR', 'Create tarball with dev documentaionl', '..', PathOption.PathAccept)
     )

    return opts



def prepareOptionsHelp(opts,env):
    prelude = '''
USAGE:   scons [-c] [OPTS] [key=val,...] [TARGETS]
     Build and optionally install Cinelerra.
     Without specifying any target, just the (re)build target will run.
     Add -c to the commandline to clean up anything a given target would produce

Special Targets:
     build   : just compile and link
     install : install created artifacts at PREFIX
     src.tar : create source tarball
     doc.tar : create developer doc tarball
     tar     : create all tarballs

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
    t = Tarball(env,location='$SRCTAR',dirs='$SRCDIR')
    artifacts['src.tar'] = t
    env.Alias('src.tar', t)
    env.Alias('tar', t)

    t =  Tarball(env,location='$DOCTAR',suffix='-doc',dirs='admin doc wiki uml tests')
    artifacts['doc.tar'] = t
    env.Alias('doc.tar', t)
    env.Alias('tar', t)



def defineBuildTargets(env, artifacts):
    ''' define the source file/dirs comprising each artifact to be built.
        setup sub-environments with special build options if necessary.
        We use a custom function to declare a whole tree of srcfiles. 
    '''
    cinobj = ( srcSubtree(env,'backend') 
             + srcSubtree(env,'proc')
             + env.Object('$SRCDIR/main.cpp')
             )
    plugobj = srcSubtree(env,'plugin', isShared=True)
    
    artifacts['cinelerra'] = env.Program('$BINDIR/cinelerra', cinobj)
    artifacts['plugins']   = env.SharedLibrary('$BINDIR/cinelerra-plugin', plugobj)
    
    # call subdir SConscript(s) for independent components
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
    
    env.Alias('build', '$BINDIR')
    env.Default('build')
    # additional files to be cleaned when cleaning 'build'
    env.Clean ('build', [ 'scache.conf', '.sconf_temp', '.sconsign.dblite', 'config.log'])

#####################################################################





### === MAIN === ####################################################


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
# 'doc.tar'     : uml model, wiki, dev docu (no src)

definePackagingTargets(env, artifacts)
defineBuildTargets(env, artifacts)
defineInstallTargets(env, artifacts)

