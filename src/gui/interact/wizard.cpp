/*
  Wizard  -  controller user help and assistance

  Copyright (C)         Lumiera.org
    2017,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file wizard.cpp
 ** Implementation of the global help controller to show the user manual.
 ** 
 ** @todo WIP 3/2017 early draft of the UI top-level controllers
 */


#include "gui/gtk-base.hpp"
#include "gui/config-keys.hpp"
#include "gui/interact/wizard.hpp"
#include "gui/interact/spot-locator.hpp"
#include "gui/workspace/workspace-window.hpp"
#include "gui/ctrl/global-ctx.hpp"
#include "lib/format-string.hpp"
//#include "lib/util.hpp"

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <vector>
#include <string>

//using util::cStr;
//using util::isnil;
using ::util::_Fmt;
using std::vector;
using std::string;


namespace gui {
namespace interact {
  
  using boost::algorithm::is_any_of;
  using boost::algorithm::split;
  using workspace::WorkspaceWindow;
  using ctrl::GlobalCtx;
  using Gtk::AboutDialog;
  
  
  // dtors via smart-ptr invoked from here... (TODO any to come here?)
  Wizard::~Wizard()
    { }
  
  
  Wizard::Wizard (GlobalCtx& globals)
    : globalCtx_{globals}
    { }
  
  
  
  /**
   * show the notorious "about Lumiera" dialog.
   * Something every application has and no user really cares about.
   */
  void
  Wizard::show_HelpAbout()
  {
    // Configure the about dialog
    AboutDialog dialog;
    
    cuString copyrightNotice {_Fmt(_("© %s the original Authors\n"
                                     "-- Lumiera Team --\n"
                                     "Lumiera is Free Software (GPL)"))
                                     % Config::get (KEY_COPYRIGHT)};
    
    string authors = Config::get (KEY_AUTHORS);
    vector<uString> authorsList;
    split (authorsList, authors, is_any_of (",|"));
    
    
    dialog.set_program_name(Config::get (KEY_TITLE));
    dialog.set_version(Config::get (KEY_VERSION));
    dialog.set_copyright(copyrightNotice);
    dialog.set_website(Config::get (KEY_WEBSITE));
    dialog.set_authors(authorsList);
    
    WorkspaceWindow& currentWindow = globalCtx_.windowList_.findActiveWindow();
    dialog.set_transient_for (currentWindow);
    
    // Show the about dialog
    dialog.run();
  }
  
  
}}// namespace gui::interact
