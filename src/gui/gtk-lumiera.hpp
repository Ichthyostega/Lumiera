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

namespace lumiera {
namespace gui {

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

  /**
   *  The main application class.
   */
  class GtkLumiera
    {
    public:
      int main(int argc, char *argv[]);

    };

  /**
   *  Returns a reference to the global application object
   */
  GtkLumiera& application();

}   // namespace gui
}   // namespace lumiera

#endif // GTK_LUMIERA_HPP


