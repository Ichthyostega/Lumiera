# coding: utf-8
##
## BuilderGCH.py  -  SCons builder for gcc's precompiled headers
##

#  Copyright (C)      scons.org/wiki/GchBuilder
#    2006,            Tim Blechmann
#    2008,            Hermann Vosseler <Ichthyostega@web.de>
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU General Public License as
#  published by the Free Software Foundation; either version 2 of
#  the License, or (at your option) any later version.
#####################################################################

# history: 8/2008 adapted for Lumiera build system
#                 changed to accept a list of precompiled header defs 

# TODO: WIP-remove these comments when port to Python 3.x is DONE 
# types.py does not contain ListType in Python 3.x
# # Why do we require ListType?
# To do something like
#    if type(item) is ListType:
#        ....
# But types.py does not have ListType anymore
#    if isinstance(item, list):
#from types import ListType

import SCons.Action
import SCons.Builder
import SCons.Scanner.C
import SCons.Util
import SCons.Script

SCons.Script.EnsureSConsVersion(0,96,92)

GchAction = SCons.Action.Action('$GCHCOM', '$GCHCOMSTR')
GchShAction = SCons.Action.Action('$GCHSHCOM', '$GCHSHCOMSTR')

def gen_suffix(env, sources):
    return sources[0].get_suffix() + env['GCHSUFFIX']


GchShBuilder = SCons.Builder.Builder(action = GchShAction,
                                     source_scanner = SCons.Scanner.C.CScanner(),
                                     suffix = gen_suffix)

GchBuilder = SCons.Builder.Builder(action = GchAction,
                                   source_scanner = SCons.Scanner.C.CScanner(),
                                   suffix = gen_suffix)

def setup_dependency(target,source,env, key):
    scanner = SCons.Scanner.C.CScanner()
    path = scanner.path(env)
    deps = scanner(source[0], env, path)

    if key in env and env[key]:
        for header in env[key]:
            header_path = header.path.strip('.gch')
            if header_path in [x.path for x in deps]:
                print("Precompiled header(%s) %s  \t <--- %s" % (key,header_path,source[0]))
                env.Depends(target, header)


def static_pch_emitter(target,source,env):
    SCons.Defaults.StaticObjectEmitter( target, source, env )
    setup_dependency(target,source,env, key='GCH')
    return (target, source)

def shared_pch_emitter(target,source,env):
    SCons.Defaults.SharedObjectEmitter( target, source, env )
    setup_dependency(target,source,env, key='GCH-sh')
    return (target, source)


def generate(env):
    """ Add builders and construction variables for the Gch builder.
    """
    env.Append(BUILDERS = {
        'gch': env.Builder(
        action = GchAction,
        target_factory = env.fs.File,
        ),
        'gchsh': env.Builder(
        action = GchShAction,
        target_factory = env.fs.File,
        ),
        })
    
    try:
        bld   = env['BUILDERS']['GCH']
        bldsh = env['BUILDERS']['GCH-sh']
    except KeyError:
        bld = GchBuilder
        bldsh = GchShBuilder
        env['BUILDERS']['PrecompiledHeader'] = bld
        env['BUILDERS']['PrecompiledHeaderShared'] = bldsh
    
    env['GCHCOM']     = '$CXX -o $TARGET -x c++-header -c $CXXFLAGS $_CCCOMCOM $SOURCE'
    env['GCHSHCOM']   = '$CXX -o $TARGET -x c++-header -c $SHCXXFLAGS $_CCCOMCOM $SOURCE'
    env['GCHSUFFIX']  = '.gch'
    
    for suffix in SCons.Util.Split('.c .C .cc .cxx .cpp .c++'):
        env['BUILDERS']['StaticObject'].add_emitter( suffix, static_pch_emitter )
        env['BUILDERS']['SharedObject'].add_emitter( suffix, shared_pch_emitter )


def exists(env):
    return env.Detect('g++')
