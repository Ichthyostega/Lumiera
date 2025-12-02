/*
  InfoBoxPanel  -  A dockable panel to expose information and parameters

   Copyright (C)
     2017,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file infobox-panel.cpp
 ** Implementation of a (dockable) panel to display and manipulate parameters.
 */

#include "stage/gtk-base.hpp"
#include "stage/panel/infobox-panel.hpp"
#include "stage/widget/error-log-display.hpp"


namespace stage  {
namespace panel{
  
  using widget::ErrorLogDisplay;
  
  
  InfoBoxPanel::InfoBoxPanel (workspace::PanelManager& panelManager, Gdl::DockItem& dockItem)
    : Panel(panelManager, dockItem, getTitle(), getStockID())
    , twoParts_{Gtk::ORIENTATION_VERTICAL}
    , buttons_{}
    , frame_{_("System Information")}
    , logExpander_{_("Error Log")}
    , theLog_{}
    {
      twoParts_.pack_start(frame_);
      twoParts_.pack_start(buttons_, Gtk::PACK_SHRINK);
      
      buttons_.set_layout (Gtk::BUTTONBOX_START);
      
      // buttons to control the error log
      buttonClear_.set_label (_("_clear Log"));
      buttonClear_.set_use_underline();
      buttonClear_.set_tooltip_markup (_("Discard all contents of the error log."));
      buttonClear_.signal_clicked().connect(
                    [this](){ if (theLog_) theLog_->clearAll(); });
      buttonClearErr_.set_label (_("_Error OK"));
      buttonClearErr_.set_use_underline();
      buttonClearErr_.set_tooltip_markup (_("Clear the error state and turn errors in to information entries."));
      buttonClearErr_.signal_clicked().connect(
                    [this](){ if (theLog_) theLog_->turnError_into_InfoMsg(); });
      buttonClearInfo_.set_label (_("drop _Info"));
      buttonClearInfo_.set_use_underline();
      buttonClearInfo_.set_tooltip_markup (_("Discard all mere info message, retain error entries only."));
      buttonClearInfo_.signal_clicked().connect(
                    [this](){ if (theLog_) theLog_->clearInfoMsg(); });
      
      buttons_.add (buttonClear_);
      buttons_.add (buttonClearErr_);
      buttons_.add (buttonClearInfo_);
      //(End)buttons...
      
      // show initial configuration....
      this->add (twoParts_);
      this->show_all();

      // schedule state update to hide the error related buttons
      // after the UI is actually mapped to screen.
      Glib::signal_idle()
        .connect_once ( sigc::bind<bool>(
                        sigc::mem_fun (*this, &InfoBoxPanel::reflect_LogErrorState), false)
                      );
    }
  
  
  const char*
  InfoBoxPanel::getTitle()
  {
    return _("InfoBox");
  }
  
  const gchar*
  InfoBoxPanel::getStockID()
  {
    return "panel_infobox";
  }
  
  
  /** on demand allocate display of information / error log
   * @note we assume it stays alive until the panel itself is closed.
   * @remark the ErrorLogDisplay widget exposes a \ref Expander functor,
   *         which is wired here with the Gtk::Expander container holding the Log.
   *         This setup allows to trigger the expand/collapse functionality and
   *         query the expansion state directly on the widget. Especially
   *         \ref NotificationHub relies on this configuration.
   */
  ErrorLogDisplay&
  InfoBoxPanel::getLog()
  {
    if (not theLog_)
      {
        theLog_.reset (new ErrorLogDisplay{});
        logExpander_.set_expanded (false);
        logExpander_.add (*theLog_);
        theLog_->expand = model::Expander{[&]() -> bool { return logExpander_.get_expanded(); }
                                         ,[&](bool yes) { logExpander_.set_expanded (yes); }
                                         };
        theLog_->reveal = model::Revealer{[&]()         { Panel::show(true);
                                                          theLog_->expand(true);
                                         }              };
        frame_.set_border_width (5);
        frame_.add (logExpander_);
        frame_.show_all();
        
        theLog_->signalErrorChanged().connect(
                    mem_fun(*this, &InfoBoxPanel::reflect_LogErrorState));
      }
    return *theLog_;
  }
  
  
  void
  InfoBoxPanel::reflect_LogErrorState (bool isError)
  {
    buttonClearErr_.set_visible (isError);
    buttonClearInfo_.set_visible (isError);
  }
  
  
}}// namespace stage::panel
