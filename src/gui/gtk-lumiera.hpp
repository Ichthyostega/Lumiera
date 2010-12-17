/*
  gtk-lumiera.hpp  -  Application wide global definitions

  Copyright (C)         Lumiera.org
    2008,               Joel Holdsworth <joel@airwebreathe.org.uk>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of
  the License, or (at your option) any later version.

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

#include <locale>
#include <gtkmm.h>
#include <nobug.h>               // need to include this after gtkmm.h, because types.h from GTK tries to shaddow the ERROR macro from windows, which kills NoBug's ERROR macro
#include <vector>
#include <boost/utility.hpp>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "lib/util.hpp"
#include "lib/lumitime.hpp"

#include "window-manager.hpp"

extern "C" {
#include <gavl/gavltime.h>
}

//NOBUG_DECLARE_FLAG(gui);

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
 * Lumiera GTK GUI implementation root.
 */
namespace gui {
  
/* ===== The Application Class ===== */

/**
 *  The main application class.
 */
class GtkLumiera : private boost::noncopyable
{
public:
  void main(int argc, char *argv[]);
  
  WindowManager& get_window_manager();
  
  static Glib::ustring get_home_data_path();

  /**
   * Returns the name of the application 
   **/
  static const Glib::ustring get_app_title();

  /**
   * Returns the version number of the application 
   **/
  static const Glib::ustring get_app_version();

  /**
   * Returns the copyright of the application
   **/
  static const Glib::ustring get_app_copyright();

  /**
   * Returns the website of the application
   **/
  static const Glib::ustring get_app_website();

  /**
   * Returns tn alphabetical list of the application's authors
   **/
  static const std::vector<Glib::ustring> get_app_authors();
  
protected:
  /**
   * The application window manager object
   **/
  WindowManager windowManager;
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

/**
 * The namespace of utility functions and classes.
 */
namespace util {}

}   // namespace gui

#endif // GTK_LUMIERA_HPP


