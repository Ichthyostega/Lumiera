/*
  GTK-BASE.hpp  -  GTK includes and basic definitions

   Copyright (C)
     2008,            Joel Holdsworth <joel@airwebreathe.org.uk>
     2011,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file gtk-base.hpp
 ** A set of basic GTK includes for the UI.
 ** There are some tricky points to consider when including the
 ** basic GTKmm headers. Especially, GTK tries to shadow the ERROR macro
 ** from Microsoft Windows. Unfortunately this breaks the ERROR macro from NoBug;
 ** thus we need to include NoBug _after_ GTK
 ** 
 ** Besides, this header defines the basic NLS. Most parts of the GUI rely on this header.
 ** 
 ** 
 ** # configuration and resource search
 ** 
 ** The GUI backbone retrieves the necessary configuration values from lumiera::Config,
 ** the config facade in the application core. Currently as of 2/2011 these values are
 ** loaded from setup.ini, because the full-blown config system is not yet implemented.
 ** Amongst others, this configuration defines a _search path_ for icons and a
 ** separate search path for resources. These path specs may use the token `$ORIGIN`
 ** to refer to the installation directory of the currently executing program.
 ** This allows for a relocatable Lumiera installation bundle.
 ** 
 ** @warning include here only what is absolutely necessary,
 **          for sake of compilation times and (debug) executable size.
 ** 
 ** @see gtk-lumiera.cpp the plugin to pull up this GUI
 ** @see stage::GuiFacade access point for starting the GUI
 ** @see stage::GuiNotification interface for communication with the gui from the lower layers
 ** @see lumiera::Config
 ** @see lumiera::BasicSetup definition of the acceptable configuration values
 ** @see lumiera::AppState general Lumiera application main
 */


#ifndef STAGE_GTK_BASE_H
#define STAGE_GTK_BASE_H

//--------------------tricky special Include sequence
#include "lib/hash-standard.hpp"
#include <locale>
#include <gtkmm.h>
#include <nobug.h>
//--------------------tricky special Include sequence

#include "lib/error.hpp"
#include "lib/integral.hpp"


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
namespace error {
  /* some commonly used error marks for the UI... */
  LUMIERA_ERROR_DECLARE (UIWIRING); // "GUI state contradicts assumptions in signal wiring"
}}



/* ======= Namespace Definitions ======= */

/**
 * Lumiera GTK UI implementation root.
 * @ingroup gui
 */
namespace stage {
  
  /* widely used type abbreviations */
  using uString =  Glib::ustring;
  using cuString = const uString;
  
  using PStyleContext = Glib::RefPtr<Gtk::StyleContext>;
  using PCairoContext = Cairo::RefPtr<Cairo::Context>;
  
  
  

  /** Backbone of the Lumiera GTK UI.
   *  All interface elements and controllers of global relevance
   *  are connected to a communication structure known as UI-Bus.
   *  This structure allows to address interface elements by ID
   *  and to process command invocations with asynchronous feedback
   *  in a uniform way.
   */
  namespace ctrl {}
  
  /** UI interaction control.
   *  Any overarching concerns of interaction patterns,
   *  selecting the subject, forming and binding of commands,
   *  management of focus and perspective, keybindings and gestures.
   */
  namespace interact {}
  
  /** The Lumiera GTK-GUI uses a thin proxy layer data model
   *  on top of the actual "high-level-model", which lives in the
   *  Steam-Layer below. Stage operations interact with these proxy
   *  model entities, which in turn forward the calls to the actual objects
   *  in the Steam-Layer, through the Command system (which enables UNDO).
   *  
   *  @todo: as of 1/2011 this connection between the GUI proxy model and
   *         the Steam-Layer model needs to be set up. Currently, the GUI model
   *         entities are just created standalone and thus dysfunctional.
   *  @todo: as of 11/2015 this connection between UI and Steam-Layer is actually
   *         in the works, and it is clear by now that we won't use proxy objects,
   *         but rather a generic bus-like connection and symbolic IDs to designate
   *         the model elements
   *  @todo: and as of 2/2017 the new structure is basically set, but we need actually
   *         to implement a new timeline display in accordance to this reworked design.
   *         After this is done, most of the model elements in this packare are obsolete.
   */
  namespace model {}
  
  /** Video output implementation. */
  namespace output {}

  /** Dialog box classes. */
  namespace dialog {}
  
  /** Docking panel classes. */
  namespace panel {}
  
  /** The asset management, configuration and project settings. */
  namespace setting {}
  
  /** The timeline display and editing operations. */
  namespace timeline {}
  
  /** The global workspace with top-level windows. */
  namespace workspace {}
  
  /** Lumiera custom widgets. */
  namespace widget {}
  
  /** Display and drawing helpers and utilities. */
  namespace draw {}
  
  /** generic definitions for element ID and access.
   * @see id-scheme.hpp
   */
  namespace idi {}
  
  
}// namespace stage
#endif /*STAGE_GTK_BASE_H*/
