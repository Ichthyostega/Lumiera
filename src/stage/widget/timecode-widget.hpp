/*
  TIMECODE-WIDGET.hpp  -  widget for timecode display / input

  Copyright (C)         Ardour.org
    1999,               Paul Davis 

  Copyright (C)         Lumiera.org
    2010,               Stefan Kangas <skangas@skangas.se

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


/** @file timecode-widget.hpp
 ** Widget for timecode display and input.
 ** Ported and adapted from the Ardour DAW.
 ** @todo needs extensive rework to get smooth integration
 **       with Lumiera's timecode handling functions 
 */


#ifndef GUI_WIDGET_TIMECODE_WIDGET_H
#define GUI_WIDGET_TIMECODE_WIDGET_H

#include "stage/gtk-base.hpp"
#include "lib/time/timevalue.hpp"

#include <gtkmm/box.h>
#include <gtkmm/menu.h>
#include <gtkmm/eventbox.h>
#include <gtkmm/label.h>
#include <gtkmm/frame.h>

#include <string>


namespace gui {
namespace widget {
  
  using lib::time::Time;
  using lib::time::TimeVar;
  using lib::time::TimeValue;
  
  using std::string;
  
  
  /**
   * Widget for timecode input and display.
   * Adapted implementation from the Ardour DAW
   */
  class TimeCode
    : public Gtk::HBox
    {
    public:
      enum Mode {
        SMPTE,
        MinSec,
        Frames,
        Off
      };
      
      
      TimeCode (string clock_name
               ,string widget_name
               ,bool editable
//             , bool is_duration = false
               );
      
      
      Mode mode()  const { return _mode; }
      
      void focus();
      
      void set(Time when, bool force = false);
      void set_mode(Mode);
      
      void set_widget_name(string);
      
      string name()  const { return _name; }
      
      Time current_time (Time position = Time::ZERO)  const;
      Time current_duration (Time position = Time::ZERO)  const;
      
      sigc::signal<void> ValueChanged;
      sigc::signal<void> ChangeAborted;
      
      static sigc::signal<void> ModeChanged;
      // static std::vector<TimeCode*> clocks;
      
      static bool has_focus() { return _has_focus; }
      
      
    private:
      uint   key_entry_state;
      Mode   _mode;
      string _name;
      // bool is_duration;
      bool   editable;
      
      Gtk::Menu  *ops_menu;
      
      Gtk::HBox   smpte_packer_hbox;
      Gtk::HBox   smpte_packer;
      
      Gtk::HBox   minsec_packer_hbox;
      Gtk::HBox   minsec_packer;
      
      Gtk::HBox   frames_packer_hbox;
      Gtk::HBox   frames_packer;
      
      enum Field {
        SMPTE_Hours,
        SMPTE_Minutes,
        SMPTE_Seconds,
        SMPTE_Frames,
        MS_Hours,
        MS_Minutes,
        MS_Seconds,
        VFrames
      };
      
      Gtk::EventBox  audio_frames_ebox;
      Gtk::Label     audio_frames_label;
      
      Gtk::HBox      off_hbox;
      
      Gtk::EventBox  hours_ebox;
      Gtk::EventBox  minutes_ebox;
      Gtk::EventBox  seconds_ebox;
      Gtk::EventBox  frames_ebox;
      
      Gtk::EventBox  ms_hours_ebox;
      Gtk::EventBox  ms_minutes_ebox;
      Gtk::EventBox  ms_seconds_ebox;
      
      Gtk::Label  hours_label;
      Gtk::Label  minutes_label;
      Gtk::Label  seconds_label;
      Gtk::Label  frames_label;
      Gtk::Label  colon1, colon2, colon3;
      
      Gtk::Label  ms_hours_label;
      Gtk::Label  ms_minutes_label;
      Gtk::Label  ms_seconds_label;
      Gtk::Label  colon4, colon5;
      
      Gtk::EventBox  clock_base;
      Gtk::Frame     clock_frame;
      
      TimeVar last_when;
      bool last_pdelta;
      bool last_sdelta;
      
      int last_hrs;
      int last_mins;
      int last_secs;
      int last_frames;
      bool last_negative;
      
      int  ms_last_hrs;
      int  ms_last_mins;
      float ms_last_secs;
      
      bool dragging;
      double drag_start_y;
      double drag_y;
      double drag_accum;
      
      void on_realize();
      
      bool field_motion_notify_event (GdkEventMotion *ev, Field);
      bool field_button_press_event (GdkEventButton *ev, Field);
      bool field_button_release_event (GdkEventButton *ev, Field);
      bool field_button_scroll_event (GdkEventScroll *ev, Field);
      bool field_key_press_event (GdkEventKey *ev, Field);
      bool field_key_release_event (GdkEventKey *ev, Field);
      bool field_focus_gain_event (GdkEventFocus*, Field);
      bool field_focus_loss_event (GdkEventFocus*, Field);
      bool drop_focus_handler (GdkEventFocus *ev);
      
      void set_smpte (Time, bool);
      void set_minsec (Time, bool);
      void set_frames (Time, bool);
      
      int get_frames (Field, Time pos = Time::ZERO, int dir=1);
      
      void smpte_sanitize_display();
      Time smpte_time_from_display()  const;
      Time minsec_time_from_display() const;
      Time audio_time_from_display()  const;
      
      void build_ops_menu();
      void setup_events();
      
      void smpte_offset_changed();
      void set_size_requests();
      
      static const uint field_length[(int)VFrames+1];
      static bool _has_focus;
      
      void set_size_request_to_display_given_text (Gtk::Widget &w, const gchar *text, gint hpadding, gint vpadding);
      void get_ink_pixel_size (Glib::RefPtr<Pango::Layout> layout, int& width, int& height);
    };
  
  
  
}}// gui::widget
#endif /*GUI_WIDGET_TIMECODE_WIDGET_H*/
