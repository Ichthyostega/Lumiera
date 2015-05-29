/*
  GtkLumiera  -  The Lumiera GUI Application Object

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

* *****************************************************/


#include "gui/gtk-lumiera.hpp"
#include "gui/window-manager.hpp"
#include "gui/workspace/workspace-window.hpp"
#include "gui/ui-bus.hpp"
#include "gui/model/project.hpp"
#include "lib/format-string.hpp"
#include "lib/depend.hpp"
#include "lib/symbol.hpp"

#include "include/config-facade.h"

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <string>
#include <vector>

namespace gui {
  
  using namespace Gtk;   ////////////////////TODO explicit usings please!
  using namespace Glib;
  using namespace gui::model;
  using namespace gui::workspace;
  using namespace gui::controller;
  
  using boost::algorithm::is_any_of;
  using boost::algorithm::split;
  
  using lumiera::Config;
  using lib::Literal;
  using ::util::_Fmt;
  using std::string;
  
  typedef std::vector<uString> UVector;
  
  
  
  namespace {
    
    /** storage for the Main Application object */
    lib::Depend<GtkLumiera> theApplicationInstance;
    
    Literal KEY_TITLE      = "Lumiera.title";
    Literal KEY_VERSION    = "Lumiera.version";
    Literal KEY_WEBSITE    = "Lumiera.website";
    Literal KEY_AUTHORS    = "Lumiera.authors";
    Literal KEY_COPYRIGHT  = "Lumiera.copyright";
    
    Literal KEY_STYLESHEET = "Gui.stylesheet";
    Literal KEY_UIRES_PATH = "Gui.resourcepath";
    Literal KEY_ICON_PATH  = "Gui.iconpath";
  }
  
  
  
  
  GtkLumiera&
  GtkLumiera::application()
  {
    return theApplicationInstance();
  }
  
  
  
  /** @remarks this function blocks until shutdown of the UI */
  void
  GtkLumiera::main (int argc, char *argv[])
  {
    Glib::thread_init();
    Gdl::init();
    Main kit(argc, argv);
    
    Glib::set_application_name (getAppTitle());
    
    Project project;
    Controller controller(project);
    
    windowManagerInstance_.init (Config::get (KEY_ICON_PATH), Config::get (KEY_UIRES_PATH));
    windowManagerInstance_.setTheme (Config::get (KEY_STYLESHEET));
    
    
    windowManagerInstance_.newWindow (project, controller);
    kit.run(); // GTK event loop
  }
  
  
  WindowManager&
  GtkLumiera::windowManager()
  {
    return windowManagerInstance_;
  }
  
  
  cuString
  GtkLumiera::getAppTitle()
  {
    return Config::get (KEY_TITLE);
  }
  
  
  cuString
  GtkLumiera::getAppVersion()
  {
    return Config::get (KEY_VERSION);
  }
  
  
  cuString
  GtkLumiera::getCopyright()
  {
    return string (
      _Fmt(_("© %s The Lumiera Team\n"
             "Lumiera is Free Software (GPL)"))
            % Config::get (KEY_COPYRIGHT));
  }
  
  
  cuString
  GtkLumiera::getLumieraWebsite()
  {
    return Config::get (KEY_WEBSITE);
  }
  
  
  const UVector
  GtkLumiera::getLumieraAuthors()
  {
    string authors = Config::get (KEY_AUTHORS);
    UVector authorsList;
    
    split (authorsList, authors, is_any_of (",|"));
    return authorsList;
  }
  
  
}// namespace gui
