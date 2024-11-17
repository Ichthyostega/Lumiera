# coding: utf-8
##
## Options.py  -  SCons build: command line options and help
##

#  Copyright (C)
#    2012,            Hermann Vosseler <Ichthyostega@web.de>
#
# **Lumiera** is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the
# Free Software Foundation; either version 2 of the License, or (at your
# option) any later version. See the file COPYING for further details.
#####################################################################


from SCons.Script import PathVariable, EnumVariable, BoolVariable, Help





def defineCmdlineVariables(buildVars):
    """ several toggles and configuration variables can be set on the commandline,
        current settings will be persisted in a options cache file.
        you may define custom variable settings in a separate file. 
        Commandline will override both.
    """
    buildVars.AddVariables(
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
        ,('TESTSUITES', 'Run only test suites matching the given pattern', '')
        ,('TESTMODE',   'test suite error mode for test.sh', '')
#       ,BoolVariable('OPENGL', 'Include support for OpenGL preview rendering', False)
#       ,EnumVariable('DIST_TARGET', 'Build target architecture', 'auto', 
#                   allowed_values=('auto', 'i386', 'i686', 'x86_64' ), ignorecase=2)
        ,PathVariable('PREFIX', 'Installation dir prefix', 'usr/local', PathVariable.PathAccept)
        ,PathVariable('INSTALLDIR', 'Root output directory for install. Final installation will happen in INSTALLDIR/PREFIX/... ', '/', PathVariable.PathIsDir)
        ,PathVariable('PKGLIBDIR', 'Installation dir for plugins, defaults to PREFIX/lib/lumiera/modules', '',PathVariable.PathAccept)
        ,PathVariable('PKGDATADIR', 'Installation dir for default config, usually PREFIX/share/lumiera', '',PathVariable.PathAccept)
     )



def prepareOptionsHelp(buildVars,env):
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
    Help(prelude + buildVars.GenerateHelpText(env))



