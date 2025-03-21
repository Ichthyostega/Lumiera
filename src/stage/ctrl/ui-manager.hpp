/*
  UI-MANAGER.hpp  -  Global UI Manager

   Copyright (C)
     2008,            Joel Holdsworth <joel@airwebreathe.org.uk>
     2017,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file ui-manager.hpp
 ** Manager for global user interface concerns, framework integration and global state.
 ** The central UiManager instance is owned by the GtkLumiera (plug-in) object and has the
 ** responsibility to operate the _UI framework_. It establishes and wires the top-level entities
 ** of the UI-Layer and thus, indirectly offers services to provide Icons and other resources, to
 ** open and manage workspace windows, to form and issue (global) actions and to delve into the
 ** UI representation of top-level parts of the session model. And, last but not least, it
 ** exposes the functions to start and stop the GTK event loop. However -- most of these
 ** activities are in fact performed and maintained by delegates and sub components.
 ** 
 ** The initialisation and shutdown of the framework is handled by the ApplicationBase parent,
 ** while the constituents of the Lumiera UI backbone are allocated as member fields:
 ** - connection to the [UI-Bus](\ref ui-bus.hpp)
 ** - the global Actions available though the menu
 ** - the InteractionDirector (top-level controller)
 ** - the StyleManager
 ** - the WindowLocator
 ** 
 ** @see gtk-lumiera.hpp
 ** @see ui-bus.hpp
 */


#ifndef STAGE_CTRL_UI_MANAGER_H
#define STAGE_CTRL_UI_MANAGER_H

#include "stage/gtk-base.hpp"
#include "lib/nocopy.hpp"

#include <functional>
#include <memory>
#include <string>


namespace stage {
  
  class UiBus;
  
namespace workspace { class UiStyle; }
namespace timeline { class TimelineWidget; }
namespace ctrl {
  
  using std::unique_ptr;
  using std::string;
  
  class GlobalCtx;
  class Actions;
  class Facade;
  
  
  
  /** Framework initialisation base */
  class ApplicationBase
    : util::NonCopyable
    {
    protected:
      ApplicationBase();
     ~ApplicationBase();
    };
  
  
  /**
   * The Lumiera UI framework and backbone object.
   * Initialises the GTK and GLib framework, starts and stops the
   * GTK event loop, and manages global concerns regarding a coherent user interface.
   * Offers access to some global UI resources, and establishes further global services
   * to create workspace windows, to bind menu / command actions and to create interface
   * widgets for working with the the top-level model parts.
   * @note UiManager itself is _not a model::Controller,_ and thus not directly connected
   *       as a first-class entity to the Bus, but it operates the GlobalCtx, and thus in
   *       turn holds the interact::InteractionDirector, which corresponds to the model root.
   */
  class UiManager
    : public ApplicationBase
    , public Gtk::UIManager
    {
      using UiStyle = workspace::UiStyle;
      
      unique_ptr<GlobalCtx> globals_;
      unique_ptr<UiStyle>   uiStyle_;
      unique_ptr<Actions>   actions_;
      unique_ptr<Facade>    facade_;
      
      
    public:
      /**
       * Initialise the GTK framework and the Lumiera UI backbone.
       * There is one global UiManager instance, which is created by
       * [the Application](\ref GtkLumiera).
       * @see #performMainLoop
       */
      UiManager (UiBus& bus);
     ~UiManager ();
      
      /**
       * Set up the first top-level application window.
       * This triggers the build-up of the user interface widgets.
       */
      void createApplicationWindow();
      
      /**
       * start the GTK Main loop and thus activate the UI.
       * @note this function does not return until UI shutdown.
       */
      void performMainLoop();
      
      /**
       * Cause the main event loop to terminate, so the application as a whole unwinds.
       */
      void terminateUI();
      
      
      using Operation = std::function<void(void)>;
      /**
       * perform an action within the UI event loop (GTK loop).
       */
      void schedule (Operation&& task);
      
      
      /** @todo find a solution how to enable/disable menu entries according to focus
       *                                               /////////////////////////////////////////////////TICKET #1076  find out how to handle this properly
       */
      void updateWindowFocusRelatedActions();
      
      void allowCloseWindow (bool yes);
      
      void configureStyle (timeline::TimelineWidget const&);
      
    private:
      
    };
  
  
  
}}// namespace stage::ctrl
#endif /*STAGE_CTRL_UI_MANAGER_H*/
