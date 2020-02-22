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


from SCons.Script import Exit
from Buildhelper import isCleanupOperation, isHelpRequest




def configure(env):
    """ locate required libraries.
        setup platform specific options.
        Abort build in case of failure.
    """
    if isCleanupOperation(env) or isHelpRequest():
        return env # skip configure in these cases
    
    conf = env.Configure()
    # run all configuration checks in the build environment defined thus far
    
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
    
    if not conf.CheckLib(symbol='clock_gettime', library='rt'):  # note librt is usually installed with libc6
        problems.append('We expect the POSIX realtime extensions to be available through librt. ' +
                        'Unable to use clock_gettime()')
    
    if conf.CheckCHeader('execinfo.h'):
        conf.env.Append(CPPFLAGS = ' -DHAVE_EXECINFO_H')
    
    if conf.CheckCHeader('valgrind/valgrind.h'):
        conf.env.Append(CPPFLAGS = ' -DHAVE_VALGRIND_H')
    else:
        print 'Valgrind not found. The use of Valgrind is optional; building without.'
    
    if not conf.CheckPkgConfig('nobugmt', 201008.1):
        problems.append('Did not find NoBug [http://nobug.pipapo.org/].')
    else:
        conf.env.mergeConf('nobugmt')
    
    if not conf.CheckCXXHeader('memory'):
        problems.append('We rely on the C++11 smart-pointers.')
    
    if not conf.CheckCXXHeader('functional'):
        problems.append('We rely on the C++11 functor objects.')
    
    if not conf.CheckCXXHeader('boost/config.hpp'):
        problems.append('We need the C++ boost-libraries.')
    else:
        if not conf.CheckCXXHeader('boost/lexical_cast.hpp'):
            problems.append('We need boost::lexical_cast')
        if not conf.CheckCXXHeader('boost/format.hpp'):
            problems.append('We need boost::format (header).')
        if not conf.CheckLibWithHeader('boost_program_options','boost/program_options.hpp','C++'):
            problems.append('We need boost::program_options (including binary lib for linking).')
        if not conf.CheckLibWithHeader('boost_system','boost/system/error_code.hpp','C++'):
            problems.append('We need the boost::system support library (including binary lib).')
        if not conf.CheckLibWithHeader('boost_filesystem','boost/filesystem.hpp','C++'):
            problems.append('We need the boost::filesystem lib (including binary lib for linking).')
    
    
    if not conf.CheckPkgConfig('gavl', '1.4'):
        problems.append('Did not find Gmerlin Audio Video Lib [http://gmerlin.sourceforge.net/gavl.html].')
    else:
        conf.env.mergeConf('gavl')
    
    if not conf.CheckPkgConfig('alsa', '1.0.23'):
        problems.append('Support for ALSA sound output is required')
    
    if not conf.CheckPkgConfig('gtkmm-3.0', '3.10'):
        problems.append('Unable to configure the mm-bindings for GTK-3')
        
    if not conf.CheckPkgConfig('glibmm-2.4', '2.39'):
        problems.append('Unable to configure the mm-bindings for Glib')
    
    if not conf.CheckPkgConfig('sigc++-2.0', '2.2.10'):
        problems.append('Need the signal-slot-binding library SigC++2')
    
    if not conf.CheckPkgConfig('glib-2.0', '2.40'):
        problems.append('Need a suitable Glib version.')
    
    if not conf.CheckPkgConfig('gthread-2.0', '2.40'):
        problems.append('Need gthread support lib for Glib based thread handling.')
    
    if not conf.CheckPkgConfig('cairomm-1.0', '1.10'):
        problems.append('Unable to configure Cairo--')
    
    verGDL   = '3.8'    # lowered requirements to allow building on Ubuntu/Trusty & Mint (was originally '3.12')
    verGDLmm = '3.7.3'
    urlGDLmm = 'http://ftp.gnome.org/pub/GNOME/sources/gdlmm/'
    urlGDLmmDEB = 'http://lumiera.org/debian/'
    if not conf.CheckPkgConfig('gdl-3.0', verGDL):
        problems.append('GNOME Docking Library not found. We need at least GDL %s '
                        'and suitable C++ ("mm")-bindings (GDLmm >=%s)' % (verGDL, verGDLmm))
    if not conf.CheckPkgConfig('gdlmm-3.0', verGDLmm, alias='gdl'):
        problems.append('We need the C++ bindings for GDL by Fabien Parent: GDLmm >=%s '
                        '(either from GNOME %s or use the debian package from %s)' % 
                        (verGDLmm, urlGDLmm, urlGDLmmDEB))
    
    if not conf.CheckPkgConfig('librsvg-2.0', '2.30'):
        problems.append('Need rsvg Library for rendering icons.')
        
    if not conf.CheckCHeader(['X11/Xutil.h', 'X11/Xlib.h'],'<>'):
        problems.append('Xlib.h and Xutil.h required. Please install libx11-dev.')
    
    # NOTE the following dependencies where for the video displayer widget.
    # As of 11/2015 this is broken and disabled. Might be obsolete.... 
    if not conf.CheckPkgConfig('xv')  : problems.append('Need libXv...')
    if not conf.CheckPkgConfig('x11') : problems.append('Need X-lib...')   # for the xvdisplayer widget
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

