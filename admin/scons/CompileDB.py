# coding: utf-8
##
## CompileDB.py  -  SCons build: generate a compilation database for clangd
##

#  Copyright (C)
#    2026,            Hermann Vosseler <Ichthyostega@web.de>
#
# **Lumiera** is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the
# Free Software Foundation; either version 2 of the License, or (at your
# option) any later version. See the file COPYING for further details.
#####################################################################


from SCons.Script import COMMAND_LINE_TARGETS



def setup(env):
    """ Emit a *compilation database*, describing every compiler invocation of the
        complete build. This JSON file is the interface used by `clangd` and the other
        tools built on the Clang front-end, which need to reproduce the exact flags
        of a translation unit in order to parse it the way the build does.

        The actual work is performed by the `CompilationDatabase` builder, which SCons
        provides since 4.0: it installs an additional *emitter* on the object builders,
        capturing each command line while the dependency graph is constructed. Thus no
        compilation is involved -- `scons compiledb` takes about two seconds -- and the
        database always covers the *complete declared build*, not just the requested
        target, since the SConscript definitions are processed into a dependency tree
        on every invocation anyway.
        @return a dependency node, to be added to the `build` alias, or an empty list
                when the feature is switched off (in which case nothing at all is
                installed into the build environment).
        @note   **must be invoked after** `Platform.configure(env)`. The emitter would
                otherwise also capture the `conftest_*` compilations performed by the
                configure checks, which then show up as bogus entries below '.sconf_temp'.
                This ordering constraint is why this module is a plain helper invoked
                from the 'SConstruct' -- like `Setup`, `Options` and `Platform` -- and
                not a SCons tool loaded from the `LumieraEnvironment` constructor.
        @remark the database is written into the (relocatable) target tree, while the
                '.clangd' configuration in the project root directs the language server
                there; consumers do not search 'target/' on their own.
    """
    if not isActivated(env): return []

    env.Tool('compilation_db')                   # the builder provided by SCons
    env['COMPILATIONDB_USE_ABSPATH'] = True      # make recorded content unambiguous
    compileDB = env.CompilationDatabase('$TARGDIR/compile_commands.json')
    env.Alias('compiledb', compileDB)
    return compileDB


def isActivated(env):
    """ generation is opt-in through the (sticky) build option,
        but can also be requested for a single invocation by naming the target.
    """
    return env['COMPILEDB'] or 'compiledb' in COMMAND_LINE_TARGETS
