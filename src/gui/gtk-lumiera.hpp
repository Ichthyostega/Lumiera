/*
  gtk-lumiera.hpp  -  Application wide global definitions
 
  Copyright (C)         Lumiera.org
    2008,               Joel Holdsworth <joel@airwebreathe.org.uk>
 
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 
*/
/** @file gtk-lumiera.hpp
 ** This file contains application wide global definitions
 ** user actions.
 ** @see gtk-lumiera.cpp
 */

#ifndef GTK_LUMIERA_HPP
#define GTK_LUMIERA_HPP

#include <gtkmm.h>
#include <nobug.h>
#include <vector>
#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include "lib/util.hpp"

extern "C" {
#include <gavl/gavltime.h>
}

NOBUG_DECLARE_FLAG(gui);

#ifdef ENABLE_NLS
#  include <libintl.h>
#  define _(String) gettext (String)
#  define gettext_noop(String) String
#  define N_(String) gettext_noop (String)
#else
#  define _(String) (String)
#  define N_(String) String
#  define textdomain(Domain)
#  define bindtextdomain(Package, Directory)
#endif

/**
 * The namespace of all GUI code.
 */
namespace gui {
  
/* ===== Global Constants ===== */

/**
 *  The name of the application 
 */
static const gchar* AppTitle = "Lumiera";

/**
 *  The version number of the application 
 */
static const gchar* AppVersion = N_("0.1-dev");

/**
 *  The copyright of the application
 */
static const gchar* AppCopyright = N_("© 2008 The Lumiera Team");

/**
 *  The website of the application
 */
static const gchar* AppWebsite = "www.lumiera.org";

/**
 *  An alphabetical list of the application's authors
 */
static const gchar* AppAuthors[] = {
  "Joel Holdsworth",
  "Christian Thaeter",
  "Hermann Vosseler",
  "<Other Authors Here>"};
  
/* ===== The Application Class ===== */

/**
 *  The main application class.
 */
class GtkLumiera : private boost::noncopyable
{
public:
  int main(int argc, char *argv[]);
  
  static Glib::ustring get_home_data_path();

};

/**
 *  Returns a reference to the global application object
 */
GtkLumiera& application();

/* ===== Namespace Definitions ===== */

/**
 * The namespace of all dialog box classes.
 */
namespace dialogs {}

/**
 * The namespace of data model classes.
 */
namespace model {}

/**
 * The namespace of all video output implementations.
 */
namespace output {}

/**
 * The namespace of all docking panel classes.
 */
namespace panels {}

/**
 * The namespace of all Lumiera custom widgets.
 */
namespace widgets {}

/**
 * The namespace of the workspace window, and it's helper classes.
 */
namespace workspace {}

}   // namespace gui

#endif // GTK_LUMIERA_HPP


