/*
  Wizard  -  controller user help and assistance

   Copyright (C)
     2017,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file wizard.cpp
 ** Implementation of the global help controller to show the user manual.
 ** 
 ** @todo WIP 3/2017 early draft of the UI top-level controllers
 */


#include "stage/gtk-base.hpp"
#include "stage/config-keys.hpp"
#include "stage/interact/wizard.hpp"
#include "stage/interact/spot-locator.hpp"
#include "stage/workspace/workspace-window.hpp"
#include "stage/panel/infobox-panel.hpp"
#include "stage/dialog/test-control.hpp"
#include "stage/ctrl/notification-hub.hpp"
#include "stage/ctrl/global-ctx.hpp"
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


namespace stage {
namespace interact {
  
  using boost::algorithm::is_any_of;
  using boost::algorithm::split;
  using workspace::WorkspaceWindow;
  using ctrl::NotificationHub;
  using ctrl::GlobalCtx;
  using Gtk::AboutDialog;
  
  
  // dtors via smart-ptr invoked from here... (TODO any to come here?)
  Wizard::~Wizard()
    { }
  
  
  Wizard::Wizard (GlobalCtx& globals)
    : globalCtx_{globals}
    , notificationHub_{new NotificationHub{getErrorLogID()
                                          ,globals.uiBus_.getAccessPoint()
                                          ,[&]() -> widget::ErrorLogDisplay&
                                             {
                                               return globalCtx_.windowLoc_.locatePanel()
                                                         .find_or_create<panel::InfoBoxPanel>()
                                                         .getLog();
                                          }} }
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
    
    WorkspaceWindow& currentWindow = globalCtx_.windowLoc_.findActiveWindow();
    dialog.set_transient_for (currentWindow);
    
    // Show the about dialog
    dialog.run();
  }
  
  
  /**
   * Launch a non modal child window to trigger self-test actions.
   * This is a tool for diagnostics and development. The operations exposed here
   * allow to launch some hard wired actions and test routines, performing within
   * the regular UI environment and on equal footing with user operated controls.
   */
  void
  Wizard::launchTestCtrl()
  {
    if (testControlWindow_)
      testControlWindow_->present(); // just (re)show the existing window
    else
      testControlWindow_ = std::make_unique<dialog::TestControl> (globalCtx_.uiBus_.getAccessPoint(),
                                                                  globalCtx_.windowLoc_.findActiveWindow());
  }
  
  
  
}}// namespace stage::interact
