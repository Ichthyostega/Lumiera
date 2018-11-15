/*
  TimecodeWidget  -  widget for timecode display / input

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
 
* *****************************************************/


/** @file timecode-widget.cpp
 ** Widget for timecode display and input.
 ** Ported and adapted from the Ardour DAW
 */


#include "gui/widget/timecode-widget.hpp"

#include "lib/time/diagnostics.hpp"  ////////////TODO: temporary solution to get H:M:S components. Use TimeCode instead!

#include <boost/lexical_cast.hpp>
#include <cmath>
#include <stdint.h>
#include <cstdio> // for sprintf
#include <cstdlib>
#include <locale>
#include <string>

#include <gavl/gavl.h>
#include <sigc++/bind.h>


using boost::lexical_cast;
using sigc::mem_fun;
using sigc::bind;
using std::string;


namespace gui {
namespace widget {
  
  using namespace Gtk;       ////////////////////////////////////////////////////////////////////////////////TICKET #1071 no wildcard includes please!
  
  // TODO: frame rate should not be a constant, but instead be per sequence
  const float framerate = 25;
  
  
  sigc::signal<void> TimeCode::ModeChanged;
  
  const unsigned int TimeCode::field_length[(int) TimeCode::VFrames+1] = {
    2,   /* SMPTE_Hours */
    2,   /* SMPTE_Minutes */
    2,   /* SMPTE_Seconds */
    2,   /* SMPTE_Frames */
    2,   /* MS_Hours */
    2,   /* MS_Minutes */
    5,   /* MS_Seconds */
    10   /* VFrames */
  };
  
  
  TimeCode::TimeCode (string clock_name, string widget_name, bool allow_edit)  /*, bool duration,*/
    : _name(clock_name)
//  , is_duration(duration)
    , editable(allow_edit)
    , colon1(":")
    , colon2(":")
    , colon3(":")
    , colon4(":")
    , colon5(":")
    {
      last_when = Time::ZERO;
      last_pdelta = 0;
      last_sdelta = 0;
      key_entry_state = 0;
      ops_menu = 0;
      dragging = false;
      
      audio_frames_ebox.add(audio_frames_label);
      
      frames_packer.set_homogeneous(false);
      frames_packer.set_border_width(2);
      frames_packer.pack_start(audio_frames_ebox, false, false);
      
      frames_packer_hbox.pack_start(frames_packer, true, false);
      
      hours_ebox.add(hours_label);
      minutes_ebox.add(minutes_label);
      seconds_ebox.add(seconds_label);
      frames_ebox.add(frames_label);
      ms_hours_ebox.add(ms_hours_label);
      ms_minutes_ebox.add(ms_minutes_label);
      ms_seconds_ebox.add(ms_seconds_label);
      
      smpte_packer.set_homogeneous(false);
      smpte_packer.set_border_width(2);
      smpte_packer.pack_start(hours_ebox, false, false);
      smpte_packer.pack_start(colon1, false, false);
      smpte_packer.pack_start(minutes_ebox, false, false);
      smpte_packer.pack_start(colon2, false, false);
      smpte_packer.pack_start(seconds_ebox, false, false);
      smpte_packer.pack_start(colon3, false, false);
      smpte_packer.pack_start(frames_ebox, false, false);
      
      smpte_packer_hbox.pack_start(smpte_packer, true, false);
      
      minsec_packer.set_homogeneous(false);
      minsec_packer.set_border_width(2);
      minsec_packer.pack_start(ms_hours_ebox, false, false);
      minsec_packer.pack_start(colon4, false, false);
      minsec_packer.pack_start(ms_minutes_ebox, false, false);
      minsec_packer.pack_start(colon5, false, false);
      minsec_packer.pack_start(ms_seconds_ebox, false, false);
      
      minsec_packer_hbox.pack_start(minsec_packer, true, false);
      
      clock_frame.set_shadow_type(Gtk::SHADOW_IN);
      clock_frame.set_name("BaseFrame");
      
      clock_frame.add(clock_base);
      
      set_widget_name(widget_name);
      
      // Set mode to force update
      _mode = Off;
      set_mode(SMPTE);
      
      pack_start(clock_frame, true, true);
      
      /* the clock base handles button releases for menu popup regardless of
         editable status. if the clock is editable, the clock base is where
         we pass focus to after leaving the last editable "field", which
         will then shutdown editing till the user starts it up again.
         
         it does this because the focus out event on the field disables
         keyboard event handling, and we don't connect anything up to
         notice focus in on the clock base. hence, keyboard event handling
         stays disabled.
      */
      
      clock_base.add_events(Gdk::BUTTON_PRESS_MASK|Gdk::BUTTON_RELEASE_MASK|Gdk::SCROLL_MASK);
      clock_base.signal_button_release_event().connect(bind(mem_fun(
        *this, &TimeCode::field_button_release_event), SMPTE_Hours));
      
      // Session::SMPTEOffsetChanged.connect(mem_fun(*this, &TimeCode::smpte_offset_changed));
      
      if (editable) {
        setup_events();
      }
      
      set(last_when, true);
    }
  
  
  
  void
  TimeCode::set_widget_name(string name)
  {
    Widget::set_name(name);
    
    clock_base.set_name(name);
    
    audio_frames_label.set_name(name);
    hours_label.set_name(name);
    minutes_label.set_name(name);
    seconds_label.set_name(name);
    frames_label.set_name(name);
    ms_hours_label.set_name(name);
    ms_minutes_label.set_name(name);
    ms_seconds_label.set_name(name);
    hours_ebox.set_name(name);
    minutes_ebox.set_name(name);
    seconds_ebox.set_name(name);
    frames_ebox.set_name(name);
    audio_frames_ebox.set_name(name);
    ms_hours_ebox.set_name(name);
    ms_minutes_ebox.set_name(name);
    ms_seconds_ebox.set_name(name);
    
    colon1.set_name(name);
    colon2.set_name(name);
    colon3.set_name(name);
    colon4.set_name(name);
    colon5.set_name(name);
    
    queue_draw();
  }
  
  
  void
  TimeCode::setup_events()
  {
    clock_base.set_can_focus(true);
    
    const Gdk::EventMask eventMask =
      Gdk::BUTTON_PRESS_MASK|
      Gdk::BUTTON_RELEASE_MASK|
      Gdk::KEY_PRESS_MASK|
      Gdk::KEY_RELEASE_MASK|
      Gdk::FOCUS_CHANGE_MASK|
      Gdk::POINTER_MOTION_MASK|
      Gdk::SCROLL_MASK;
    
    hours_ebox.add_events(eventMask);
    minutes_ebox.add_events(eventMask);
    seconds_ebox.add_events(eventMask);
    frames_ebox.add_events(eventMask);
    ms_hours_ebox.add_events(eventMask);
    ms_minutes_ebox.add_events(eventMask);
    ms_seconds_ebox.add_events(eventMask);
    audio_frames_ebox.add_events(eventMask);
    
    hours_ebox.set_can_focus(true);
    minutes_ebox.set_can_focus(true);
    seconds_ebox.set_can_focus(true);
    frames_ebox.set_can_focus(true);
    audio_frames_ebox.set_can_focus(true);
    ms_hours_ebox.set_can_focus(true);
    ms_minutes_ebox.set_can_focus(true);
    ms_seconds_ebox.set_can_focus(true);
    
    
    auto connect_motion_event = [=] (EventBox& guiElm, Field fieldID)
      {
        auto handlerSlot = bind (mem_fun(this, &TimeCode::field_motion_notify_event), fieldID);
        guiElm.signal_motion_notify_event().connect (handlerSlot);
      };
    
    connect_motion_event (hours_ebox,   SMPTE_Hours);
    connect_motion_event (minutes_ebox, SMPTE_Minutes);
    connect_motion_event (seconds_ebox, SMPTE_Seconds);
    connect_motion_event (frames_ebox,  SMPTE_Frames);
    
    connect_motion_event (audio_frames_ebox, VFrames);
    
    connect_motion_event (ms_hours_ebox,   MS_Hours);
    connect_motion_event (ms_minutes_ebox, MS_Minutes);
    connect_motion_event (ms_seconds_ebox, MS_Seconds);
    
    
    auto connect_button_press = [=] (EventBox& guiElm, Field fieldID)
      {
        auto handlerSlot = bind (mem_fun(this, &TimeCode::field_button_press_event), fieldID);
        guiElm.signal_button_press_event().connect (handlerSlot);
      };
    
    connect_button_press (hours_ebox,   SMPTE_Hours);
    connect_button_press (minutes_ebox, SMPTE_Minutes);
    connect_button_press (seconds_ebox, SMPTE_Seconds);
    connect_button_press (frames_ebox,  SMPTE_Frames);
    
    connect_button_press (audio_frames_ebox, VFrames);
    
    connect_button_press (ms_hours_ebox,   MS_Hours);
    connect_button_press (ms_minutes_ebox, MS_Minutes);
    connect_button_press (ms_seconds_ebox, MS_Seconds);
    
    
    auto connect_button_release = [=] (EventBox& guiElm, Field fieldID)
      {
        auto handlerSlot = bind (mem_fun(this, &TimeCode::field_button_release_event), fieldID);
        guiElm.signal_button_release_event().connect (handlerSlot);
      };
    
    connect_button_release (hours_ebox,   SMPTE_Hours);
    connect_button_release (minutes_ebox, SMPTE_Minutes);
    connect_button_release (seconds_ebox, SMPTE_Seconds);
    connect_button_release (frames_ebox,  SMPTE_Frames);
    
    connect_button_release (audio_frames_ebox, VFrames);
    
    connect_button_release (ms_hours_ebox,   MS_Hours);
    connect_button_release (ms_minutes_ebox, MS_Minutes);
    connect_button_release (ms_seconds_ebox, MS_Seconds);
    
    
    auto connect_scroll_event = [=] (EventBox& guiElm, Field fieldID)
      {
        auto handlerSlot = bind (mem_fun(this, &TimeCode::field_button_scroll_event), fieldID);
        guiElm.signal_scroll_event().connect (handlerSlot);
      };
    
    connect_scroll_event (hours_ebox,   SMPTE_Hours);
    connect_scroll_event (minutes_ebox, SMPTE_Minutes);
    connect_scroll_event (seconds_ebox, SMPTE_Seconds);
    connect_scroll_event (frames_ebox,  SMPTE_Frames);
    
    connect_scroll_event (audio_frames_ebox, VFrames);
    
    connect_scroll_event (ms_hours_ebox,   MS_Hours);
    connect_scroll_event (ms_minutes_ebox, MS_Minutes);
    connect_scroll_event (ms_seconds_ebox, MS_Seconds);
    
    
    auto connect_key_press = [=] (EventBox& guiElm, Field fieldID)
      {
        auto handlerSlot = bind (mem_fun(this, &TimeCode::field_key_press_event), fieldID);
        guiElm.signal_key_press_event().connect (handlerSlot);
      };
    
    connect_key_press (hours_ebox,   SMPTE_Hours);
    connect_key_press (minutes_ebox, SMPTE_Minutes);
    connect_key_press (seconds_ebox, SMPTE_Seconds);
    connect_key_press (frames_ebox,  SMPTE_Frames);
    
    connect_key_press (audio_frames_ebox, VFrames);
    
    connect_key_press (ms_hours_ebox,   MS_Hours);
    connect_key_press (ms_minutes_ebox, MS_Minutes);
    connect_key_press (ms_seconds_ebox, MS_Seconds);
    
    
    auto connect_key_release = [=] (EventBox& guiElm, Field fieldID)
      {
        auto handlerSlot = bind (mem_fun(this, &TimeCode::field_key_release_event), fieldID);
        guiElm.signal_key_release_event().connect (handlerSlot);
      };
    
    connect_key_release (hours_ebox,   SMPTE_Hours);
    connect_key_release (minutes_ebox, SMPTE_Minutes);
    connect_key_release (seconds_ebox, SMPTE_Seconds);
    connect_key_release (frames_ebox,  SMPTE_Frames);
    
    connect_key_release (audio_frames_ebox, VFrames);
    
    connect_key_release (ms_hours_ebox,   MS_Hours);
    connect_key_release (ms_minutes_ebox, MS_Minutes);
    connect_key_release (ms_seconds_ebox, MS_Seconds);
    
    
    auto connect_focus_gain = [=] (EventBox& guiElm, Field fieldID)
      {
        auto handlerSlot = bind (mem_fun(this, &TimeCode::field_focus_gain_event), fieldID);
        guiElm.signal_focus_in_event().connect (handlerSlot);
      };
    
    connect_focus_gain (hours_ebox,   SMPTE_Hours);
    connect_focus_gain (minutes_ebox, SMPTE_Minutes);
    connect_focus_gain (seconds_ebox, SMPTE_Seconds);
    connect_focus_gain (frames_ebox,  SMPTE_Frames);
    
    connect_focus_gain (audio_frames_ebox, VFrames);
    
    connect_focus_gain (ms_hours_ebox,   MS_Hours);
    connect_focus_gain (ms_minutes_ebox, MS_Minutes);
    connect_focus_gain (ms_seconds_ebox, MS_Seconds);
    
    
    auto connect_focus_loss = [=] (EventBox& guiElm, Field fieldID)
      {
        auto handlerSlot = bind (mem_fun(this, &TimeCode::field_focus_loss_event), fieldID);
        guiElm.signal_focus_out_event().connect (handlerSlot);
      };
    
    connect_focus_loss (hours_ebox,   SMPTE_Hours);
    connect_focus_loss (minutes_ebox, SMPTE_Minutes);
    connect_focus_loss (seconds_ebox, SMPTE_Seconds);
    connect_focus_loss (frames_ebox,  SMPTE_Frames);
    
    connect_focus_loss (audio_frames_ebox, VFrames);
    
    connect_focus_loss (ms_hours_ebox,   MS_Hours);
    connect_focus_loss (ms_minutes_ebox, MS_Minutes);
    connect_focus_loss (ms_seconds_ebox, MS_Seconds);
    
    
    clock_base.signal_focus_in_event().connect(mem_fun(
      *this, &TimeCode::drop_focus_handler));
  }
  
  
  bool
  TimeCode::drop_focus_handler(GdkEventFocus*)
  {
    // Keyboard::magic_widget_drop_focus();
    return false;
  }
  
  
  void
  TimeCode::on_realize()
  {
    HBox::on_realize();
    
    /* styles are not available until the widgets are bound to a window */
    
    set_size_requests();
  }
  
  
  void
  TimeCode::set (Time when, bool force)
  {
    if (!force && when == last_when) return;
    
    switch (_mode)
      {
      case SMPTE:
        set_smpte(when, force);
        break;
      
      case MinSec:
        set_minsec(when, force);
        break;
      
      case Frames:
        set_frames(when, force);
        break;
      
      case Off:
        break;
      }
    
    last_when = when;
  }
  
  
  // void
  // TimeCode::smpte_offset_changed()
  // {
  //  gavl_time_t current;
  
  //  switch (_mode) {
  //  case SMPTE:
  //    // if(is_duration) {
  //    //  current = current_duration();
  //    // } else {
  //     current = current_time();
  //    // }
  //    set(current, true);
  //    break;
  //  default:
  //    break;
  //  }
  // }
  
  
  void
  TimeCode::set_frames (Time when, bool force)
  {
    ////////////////////////////////////TICKET #750 : integrate Timecode formats, let Digxel class do the formatting  
    
    char buf[32];
    snprintf(buf, sizeof(buf), "%u", uint(123));
    audio_frames_label.set_text(buf);
  }
  
  
  void
  TimeCode::set_minsec (Time when, bool force)
  {
    char buf[32];
                            ////////////TICKET #750 : temporary solution to get H:M:S components. Use TimeCode instead!
    int hrs    = getHours(when);
    int mins   = getMins (when);
    float secs = getSecs (when);
    
    if (force || hrs != ms_last_hrs)
      {
        sprintf(buf, "%02d", hrs);
        ms_hours_label.set_text(buf);
        ms_last_hrs = hrs;
      }
    
    if (force || mins != ms_last_mins)
      {
        sprintf(buf, "%02d", mins);
        ms_minutes_label.set_text(buf);
        ms_last_mins = mins;
      }
    
    if (force || secs != ms_last_secs)
      {
        sprintf(buf, "%06.3f", secs);
        ms_seconds_label.set_text(buf);
        ms_last_secs = secs;
      }
  }
  
  
  void
  TimeCode::set_smpte (Time when, bool force)
  {
    char buf[32];
                            ////////////TICKET #750 : temporary solution to get H:M:S components. Use TimeCode instead!
    int smpte_negative = 0; // FIXME: when < 0;
    int smpte_hours    = getHours(when);
    int smpte_minutes  = getMins(when);
    int smpte_seconds  = getSecs(when);
    int smpte_frames   = 0; //when.getFrames(framerate);
    
    // if (is_duration) {
    //  session->smpte_duration(when, smpte);
    // } else {
    // session->smpte_time(when, smpte);
    // }
    
    if (force || smpte_hours != last_hrs || smpte_negative != last_negative)
      {
        if (smpte_negative)
          {
            sprintf(buf, "-%02d", smpte_hours);
          }
        else
          {
            sprintf(buf, " %02d", smpte_hours);
          }
        hours_label.set_text(buf);
        last_hrs = smpte_hours;
        last_negative = smpte_negative;
      }
    
    if (force || smpte_minutes != last_mins)
      {
        sprintf(buf, "%02d", smpte_minutes);
        minutes_label.set_text(buf);
        last_mins = smpte_minutes;
      }
    
    if (force || smpte_seconds != last_secs)
      {
        sprintf(buf, "%02d", smpte_seconds);
        seconds_label.set_text(buf);
        last_secs = smpte_seconds;
      }
    
    if (force || smpte_frames != last_frames)
      {
        sprintf(buf, "%02d", smpte_frames);
        frames_label.set_text(buf);
        last_frames = smpte_frames;
      }
  }
  
  
  void
  TimeCode::focus()
  {
    switch (_mode)
      {
      case SMPTE:
        hours_ebox.grab_focus();
        break;
      
      case MinSec:
        ms_hours_ebox.grab_focus();
        break;
      
      case Frames:
        frames_ebox.grab_focus();
        break;
      
      case Off:
        break;
      }
  }
  
  
  bool
  TimeCode::field_key_press_event (GdkEventKey *ev, Field field)
  {
    /* all key activity is handled on key release */
    return true;
  }
  
  
  bool
  TimeCode::field_key_release_event (GdkEventKey *ev, Field field)
  {
    Label *label = 0;
    string new_text;
    char new_char = 0;
    bool move_on = false;
    
    switch (field)
      {
      case SMPTE_Hours:
        label = &hours_label;
        break;
      case SMPTE_Minutes:
        label = &minutes_label;
        break;
      case SMPTE_Seconds:
        label = &seconds_label;
        break;
      case SMPTE_Frames:
        label = &frames_label;
        break;
      
      case VFrames:
        label = &audio_frames_label;
        break;
      
      case MS_Hours:
        label = &ms_hours_label;
        break;
      case MS_Minutes:
        label = &ms_minutes_label;
        break;
      case MS_Seconds:
        label = &ms_seconds_label;
        break;
      
      default:
        return false;
      }
    
    switch (ev->keyval) {
#if false   ////////////////////////////////////////////////TODO integrate / adapt to GTK-3
    case GDK_0:
    case GDK_KP_0:
      new_char = '0';
      break;
    case GDK_1:
    case GDK_KP_1:
      new_char = '1';
      break;
    case GDK_2:
    case GDK_KP_2:
      new_char = '2';
      break;
    case GDK_3:
    case GDK_KP_3:
      new_char = '3';
      break;
    case GDK_4:
    case GDK_KP_4:
      new_char = '4';
      break;
    case GDK_5:
    case GDK_KP_5:
      new_char = '5';
      break;
    case GDK_6:
    case GDK_KP_6:
      new_char = '6';
      break;
    case GDK_7:
    case GDK_KP_7:
      new_char = '7';
      break;
    case GDK_8:
    case GDK_KP_8:
      new_char = '8';
      break;
    case GDK_9:
    case GDK_KP_9:
      new_char = '9';
      break;
    
    case GDK_period:
    case GDK_KP_Decimal:
      if (_mode == MinSec && field == MS_Seconds) {
        new_char = '.';
      } else {
        return false;
      }
      break;
    
    case GDK_Tab:
    case GDK_Return:
    case GDK_KP_Enter:
      move_on = true;
      break;
    
    case GDK_Escape:
      key_entry_state = 0;
      clock_base.grab_focus();
      ChangeAborted();  /*  EMIT SIGNAL  */
      return true;
#endif
    default:
      return false;
    }
    
    if (!move_on)
      {
        if (key_entry_state == 0)
          {
          // Initialise with a fresh new string
          if (field != VFrames)
            {
              for (unsigned int xn = 0; xn < field_length[field] - 1; ++xn)
                new_text += '0';
            }
          else
            {
              new_text = "";
            }
          }
        else
          {
            string existing = label->get_text();
            if (existing.length() >= field_length[field])
              new_text = existing.substr(1, field_length[field] - 1);
            else
              new_text = existing.substr(0, field_length[field] - 1);
          }
        
        new_text += new_char;
        label->set_text(new_text);
        key_entry_state++;
      }
    
    move_on = (key_entry_state == field_length[field]);
    
    if (move_on)
      {
        if (key_entry_state)
          {
            switch (field)
              {
              case SMPTE_Hours:
              case SMPTE_Minutes:
              case SMPTE_Seconds:
              case SMPTE_Frames:
                // Check SMPTE fields for sanity (may also adjust fields)
                smpte_sanitize_display();
                break;
              default:
                break;
              }
            
            ValueChanged(); /* EMIT_SIGNAL */
          }
        
        
        /* move on to the next field. */
        switch (field)
          {
            /* SMPTE */
            
          case SMPTE_Hours:
            minutes_ebox.grab_focus();
            break;
          case SMPTE_Minutes:
            seconds_ebox.grab_focus();
            break;
          case SMPTE_Seconds:
            frames_ebox.grab_focus();
            break;
          case SMPTE_Frames:
            clock_base.grab_focus();
            break;
          
          /* frames */
          
          case VFrames:
            clock_base.grab_focus();
            break;
          
          /* Min:Sec */
          
          case MS_Hours:
            ms_minutes_ebox.grab_focus();
            break;
          case MS_Minutes:
            ms_seconds_ebox.grab_focus();
            break;
          case MS_Seconds:
            clock_base.grab_focus();
            break;
            
          default:
            break;
        }
      }//if (move_on)
    
#if false /////////////////////////////////////////////////TODO adapt / GTK-3 port
    //if user hit Enter, lose focus
    switch (ev->keyval) {
    case GDK_Return:
    case GDK_KP_Enter:
      clock_base.grab_focus();
    }
#endif
    return true;
  }
  
  
  bool
  TimeCode::field_focus_gain_event (GdkEventFocus*, Field field)
  {
    key_entry_state = 0;
    
    // Keyboard::magic_widget_grab_focus();
    
    switch (field) {
    case SMPTE_Hours:
      //hours_ebox.set_flags(Gtk::HAS_FOCUS);
      hours_ebox.set_state(Gtk::STATE_ACTIVE);
      break;
    case SMPTE_Minutes:
      //minutes_ebox.set_flags(Gtk::HAS_FOCUS);
      minutes_ebox.set_state(Gtk::STATE_ACTIVE);
      break;
    case SMPTE_Seconds:
      //seconds_ebox.set_flags(Gtk::HAS_FOCUS);
      seconds_ebox.set_state(Gtk::STATE_ACTIVE);
      break;
    case SMPTE_Frames:
      //frames_ebox.set_flags(Gtk::HAS_FOCUS);
      frames_ebox.set_state(Gtk::STATE_ACTIVE);
      break;
      
    case VFrames:
      ///audio_frames_ebox.set_flags(Gtk::HAS_FOCUS);
      audio_frames_ebox.set_state(Gtk::STATE_ACTIVE);
      break;
      
    case MS_Hours:
      //ms_hours_ebox.set_flags(Gtk::HAS_FOCUS);
      ms_hours_ebox.set_state(Gtk::STATE_ACTIVE);
      break;
    case MS_Minutes:
      //ms_minutes_ebox.set_flags(Gtk::HAS_FOCUS);
      ms_minutes_ebox.set_state(Gtk::STATE_ACTIVE);
      break;
    case MS_Seconds:
      //ms_seconds_ebox.set_flags(Gtk::HAS_FOCUS);
      ms_seconds_ebox.set_state(Gtk::STATE_ACTIVE);
      break;
    }
    
    return false;
  }
  
  
  bool
  TimeCode::field_focus_loss_event (GdkEventFocus*, Field field)
  {
    switch (field) {
    
    case SMPTE_Hours:
      //hours_ebox.unset_flags(Gtk::HAS_FOCUS);
      hours_ebox.set_state(Gtk::STATE_NORMAL);
      break;
    case SMPTE_Minutes:
      //minutes_ebox.unset_flags(Gtk::HAS_FOCUS);
      minutes_ebox.set_state(Gtk::STATE_NORMAL);
      break;
    case SMPTE_Seconds:
      //seconds_ebox.unset_flags(Gtk::HAS_FOCUS);
      seconds_ebox.set_state(Gtk::STATE_NORMAL);
      break;
    case SMPTE_Frames:
      //frames_ebox.unset_flags(Gtk::HAS_FOCUS);
      frames_ebox.set_state(Gtk::STATE_NORMAL);
      break;
      
    case VFrames:
      //audio_frames_ebox.unset_flags(Gtk::HAS_FOCUS);
      audio_frames_ebox.set_state(Gtk::STATE_NORMAL);
      break;
      
    case MS_Hours:
      //ms_hours_ebox.unset_flags(Gtk::HAS_FOCUS);
      ms_hours_ebox.set_state(Gtk::STATE_NORMAL);
      break;
    case MS_Minutes:
      //ms_minutes_ebox.unset_flags(Gtk::HAS_FOCUS);
      ms_minutes_ebox.set_state(Gtk::STATE_NORMAL);
      break;
    case MS_Seconds:
      //ms_seconds_ebox.unset_flags(Gtk::HAS_FOCUS);
      ms_seconds_ebox.set_state(Gtk::STATE_NORMAL);
      break;
    }
    
    // Keyboard::magic_widget_drop_focus();
    
    return false;
  }
  
  
  bool
  TimeCode::field_button_release_event (GdkEventButton *ev, Field field)
  {
    if (dragging)
      {
        gdk_pointer_ungrab(GDK_CURRENT_TIME);
        dragging = false;
        if (ev->y > drag_start_y+1 || ev->y < drag_start_y-1
            || (ev->state & GDK_SHIFT_MASK) == GDK_SHIFT_MASK)
        {
          // we actually dragged so return without setting editing focus, or we shift clicked
          return true;
        }
      }
    
    if (!editable)
      {
        if (ops_menu == 0) {
          build_ops_menu();
        }
        ops_menu->popup(1, ev->time);
        return true;
      }
    
    switch (ev->button)
      {
      case 1:
        switch (field)
        {
        case SMPTE_Hours:
          hours_ebox.grab_focus();
          break;
        case SMPTE_Minutes:
          minutes_ebox.grab_focus();
          break;
        case SMPTE_Seconds:
          seconds_ebox.grab_focus();
          break;
        case SMPTE_Frames:
          frames_ebox.grab_focus();
          break;
          
        case VFrames:
          audio_frames_ebox.grab_focus();
          break;
        
        case MS_Hours:
          ms_hours_ebox.grab_focus();
          break;
        case MS_Minutes:
          ms_minutes_ebox.grab_focus();
          break;
        case MS_Seconds:
          ms_seconds_ebox.grab_focus();
          break;
        }
        break;
        
      case 3:
        if (ops_menu == 0) {
          build_ops_menu();
        }
        ops_menu->popup(1, ev->time);
        return true;
        
      default:
        break;
      }
    
    return true;
  }
  
  
  bool
  TimeCode::field_button_press_event (GdkEventButton *ev, Field field)
  {
    return false;  ////////////////////////////////////////////////////////////////////TODO integrate time value mutation
    // if (session == 0) return false;
    // Time frames = 0;
    
    switch (ev->button)
      {
      case 1:
        // if (Keyboard::modifier_state_equals (ev->state, Keyboard::TertiaryModifier)) {
        //  set (frames, true);
        //  ValueChanged (); /* EMIT_SIGNAL */
        //      }
        
        /* make absolutely sure that the pointer is grabbed */
        gdk_pointer_grab(ev->window,false ,
                         GdkEventMask( Gdk::POINTER_MOTION_MASK | Gdk::BUTTON_PRESS_MASK |Gdk::BUTTON_RELEASE_MASK), 
                         NULL,NULL,ev->time);
        dragging = true;
        drag_accum = 0;
        drag_start_y = ev->y;
        drag_y = ev->y;
        break;
        
      case 2:
        // if (Keyboard::modifier_state_equals (ev->state, Keyboard::TertiaryModifier)) {
        //  set (frames, true);
        //  ValueChanged (); /* EMIT_SIGNAL */
        // }
        break;
        
      case 3:
        /* used for context sensitive menu */
        return false;
        break;
        
      default:
        return false;
        break;
      }
    return true;
  }
  
  bool
  TimeCode::field_button_scroll_event (GdkEventScroll *ev, Field field)
  {
    return false;
    
    // if (session == 0) {
    //  return false;
    // }
    
    // Time frames = 0;
    
    switch (ev->direction) {
    
    case GDK_SCROLL_UP:
           // frames = get_frames (field);
           // if (frames != 0) {
           //  // if (Keyboard::modifier_state_equals (ev->state, Keyboard::PrimaryModifier)) {
           //  //  frames *= 10;
           //  // }
           //  set (current_time() + frames, true);
           //  ValueChanged (); /* EMIT_SIGNAL */
           // }
           break;
    
    case GDK_SCROLL_DOWN:
           // frames = get_frames (field);
           // if (frames != 0) {
           //  // if (Keyboard::modifier_state_equals (ev->state, Keyboard::PrimaryModifier)) {
           //  //  frames *= 10;
           //  // }
           
           //  if ((double)current_time() - (double)frames < 0.0) {
           //   set (0, true);
           //  } else {
           //   set (current_time() - frames, true);
           //  }
            
           //  ValueChanged (); /* EMIT_SIGNAL */
           // }
           break;
           
    default:
      return false;
      break;
    }
    
    return true;
  }
  
  
  bool
  TimeCode::field_motion_notify_event (GdkEventMotion *ev, Field field)
  {
    if (!dragging) return false;
    
    float pixel_frame_scale_factor = 0.2f;
    
    /*
    if (Keyboard::modifier_state_equals (ev->state, Keyboard::PrimaryModifier))  {
      pixel_frame_scale_factor = 0.1f;
    }
    
    if (Keyboard::modifier_state_contains (ev->state, 
                   Keyboard::PrimaryModifier|Keyboard::SecondaryModifier)) {
      
      pixel_frame_scale_factor = 0.025f;
    }
    */
    double y_delta = ev->y - drag_y;
    
    drag_accum +=  y_delta*pixel_frame_scale_factor;
    
    drag_y = ev->y;
    
    if (trunc(drag_accum) != 0)
      {
        int frames;
        TimeVar pos(current_time());
        int dir;
        dir = (drag_accum < 0 ? 1:-1);
        frames = get_frames(field,pos,dir);
                          /////////////////////////////////////////////////////////////TICKET #750 : factor out all timecode calculations
                                                                         //////////////              and concentrate them in lib/time/timecode.cpp
        
        if (frames  != 0 &&  frames * drag_accum < (_raw(current_time())))
          {
            // minus because up is negative in computer-land
            pos = TimeValue (floor (pos - drag_accum * frames));
            set (pos, false);
          }
        else
          {
            set (Time::ZERO, false);
          }
        
        drag_accum = 0;
        ValueChanged();  /* EMIT_SIGNAL */
    }
    
    return true;
  }
  
  
  int
  TimeCode::get_frames (Field field, Time pos, int dir)
  {
    ///////////////////////////////////////////////////////TICKET #750 : integrate Timecode formats, let Digxel class do the conversions
    int frames = 0;
    // switch (field)
    //   {
    //   case SMPTE_Hours:
    //     frames = (Time) floor (3600.0 * session->frame_rate());
    //     break;
    //   case SMPTE_Minutes:
    //     frames = (Time) floor (60.0 * session->frame_rate());
    //     break;
    //   case SMPTE_Seconds:
    //     frames = session->frame_rate();
    //     break;
    //   case SMPTE_Frames:
    //     frames = (Time) floor (session->frame_rate() / session->smpte_frames_per_second());
    //     break;
    
    //   case VFrames:
    //     frames = 1;
    //     break;
    
    //   case MS_Hours:
    //     frames = (Time) floor (3600.0 * session->frame_rate());
    //     break;
    //   case MS_Minutes:
    //     frames = (Time) floor (60.0 * session->frame_rate());
    //     break;
    //   case MS_Seconds:
    //     frames = session->frame_rate();
    //     break;
    //   }
    
    return frames;
  }
  
  
  Time
  TimeCode::current_time (Time pos)  const
  {
    ///////////////////////////////////////////////////////TICKET #750 : integrate Timecode formats, let Digxel class do the conversions
    TimeVar ret;
    
    switch (_mode)
      {
      case SMPTE:
        ret = smpte_time_from_display();
        break;
        
      case MinSec:
        ret = minsec_time_from_display();
        break;
        
      case Frames:
        ret = audio_time_from_display();
        break;
        
      case Off:
        break;
      }
    
    return ret;
  }
  
  
  Time
  TimeCode::current_duration (Time pos)  const
  {
    ///////////////////////////////////////////////////////TICKET #750 : integrate Timecode formats, let Digxel class do the conversions
    TimeVar ret;
    
    switch (_mode)
      {
      case SMPTE:
        ret = smpte_time_from_display();
        break;
        
      case MinSec:
        ret = minsec_time_from_display();
        break;
        
      case Frames:
        ret = audio_time_from_display();
        break;
        
      case Off:
        break;
      }
    
    return ret;
  }
  
  
  void
  TimeCode::smpte_sanitize_display()
  {
    ///////////////////////////////////////////////////////TICKET #750 : integrate Timecode formats, let Digxel class do the conversions
    
    
    // Check SMPTE fields for sanity, possibly adjusting values
    if (59 < lexical_cast<int>(minutes_label.get_text()))
      {
        minutes_label.set_text("59");
      }
    
    if (59 < lexical_cast<int>(seconds_label.get_text()))
      {
        seconds_label.set_text("59");
      }
    
    if (framerate - 1 < lexical_cast<int>(frames_label.get_text()))
      {
        char buf[32];
        sprintf(buf, "%02d", int(framerate - 1));
        frames_label.set_text(buf);
      }
    
    //////////////////////////////////////////TODO: Drop frames // use our own Timecode handling/conversion framework through the Digxel class
    
    // if (session->smpte_drop_frames()) {
    //  if ((atoi(minutes_label.get_text()) % 10) && (atoi(seconds_label.get_text()) == 0) && (atoi(frames_label.get_text()) < 2)) {
    //    frames_label.set_text("02");
    //  }
    // }
  }
  
  
  Time
  TimeCode::smpte_time_from_display()  const
  {
    //////////////////////////////////////////TODO rewrite this to use our Digxel for timecode integration, mutation and number conversions
    
    // SMPTE::Time smpte;
    // Time sample;
    
    // smpte.hours = atoi (hours_label.get_text());
    // smpte.minutes = atoi (minutes_label.get_text());
    // smpte.seconds = atoi (seconds_label.get_text());
    // smpte.frames = atoi (frames_label.get_text());
    // smpte.rate = session->smpte_frames_per_second();
    // smpte.drop= session->smpte_drop_frames();
    
    // session->smpte_to_sample(smpte, sample, false /* use_offset */, false /* use_subframes */ );
    
    return Time::ZERO;
  }
  
  
  Time
  TimeCode::minsec_time_from_display ()  const
  {
    //////////////////////////////////////////TODO rewrite this to use our Digxel for timecode integration, mutation and number conversions
    
    // int hrs = atoi (ms_hours_label.get_text());
    // int mins = atoi (ms_minutes_label.get_text());
    // float secs = atof (ms_seconds_label.get_text());
    
    // Time sr = session->frame_rate();
    
    // return (Time) floor ((hrs * 60.0f * 60.0f * sr) + (mins * 60.0f * sr) + (secs * sr));
    
    return Time::ZERO;
  }
  
  
  Time
  TimeCode::audio_time_from_display ()  const
  {
    gavl_time_t parsedAudioFrames = lexical_cast<int>(audio_frames_label.get_text());
    return Time(TimeValue(parsedAudioFrames));
  }
  
  
  void
  TimeCode::build_ops_menu ()
  {
#if false //////////////////////////////////////////////////////////////TODO unfinished port / adapt to GTK-3
   using namespace Menu_Helpers;
   ops_menu = new Menu;
   MenuList& ops_items = ops_menu->items();
   ops_menu->set_name ("LumieraContextMenu");
    
   ops_items.push_back (MenuElem ("SMPTE", bind (mem_fun(*this, &TimeCode::set_mode), SMPTE)));
   ops_items.push_back (MenuElem ("Minutes:Seconds", bind (mem_fun(*this, &TimeCode::set_mode), MinSec)));
   ops_items.push_back (MenuElem ("Frames", bind (mem_fun(*this, &TimeCode::set_mode), Frames)));
   ops_items.push_back (MenuElem ("Off", bind (mem_fun(*this, &TimeCode::set_mode), Off)));
#endif
  }
  
  
  void
  TimeCode::set_mode(Mode m)
  {
    /* slightly tricky: this is called from within the ARDOUR_UI
       constructor by some of its clock members. at that time
       the instance pointer is unset, so we have to be careful.
       the main idea is to drop keyboard focus in case we had
       started editing the clock and then we switch clock mode.
    */
    
    clock_base.grab_focus();
    
    if (_mode == m)
      return;
    
    clock_base.remove();
    
    _mode = m;
    
    switch (_mode)
      {
      case SMPTE:
        clock_base.add(smpte_packer_hbox);
        break;
        
      case MinSec:
        clock_base.add(minsec_packer_hbox);
        break;
        
      case Frames:
        clock_base.add(frames_packer_hbox);
        break;
        
      case Off:
        clock_base.add(off_hbox);
        break;
      }
    
    set_size_requests();
    
    set(last_when, true);
    clock_base.show_all();
    key_entry_state = 0;
    
    ModeChanged(); /* EMIT SIGNAL */
  }
  
  
  void
  TimeCode::set_size_requests()
  {
    /* note that in some fonts, "88" is narrower than "00", hence the 2 pixel padding */
    
    switch (_mode) {
    case SMPTE:
      set_size_request_to_display_given_text(hours_label, "-00", 5, 5);
      set_size_request_to_display_given_text(minutes_label, "00", 5, 5);
      set_size_request_to_display_given_text(seconds_label, "00", 5, 5);
      set_size_request_to_display_given_text(frames_label, "00", 5, 5);
      break;
      
    case MinSec:
      set_size_request_to_display_given_text(ms_hours_label, "00", 5, 5);
      set_size_request_to_display_given_text(ms_minutes_label, "00", 5, 5);
      set_size_request_to_display_given_text(ms_seconds_label, "00.000", 5, 5);
      break;
      
    case Frames:
      set_size_request_to_display_given_text(audio_frames_label, "0000000000", 5, 5);
      break;
      
    case Off:
      set_size_request_to_display_given_text(off_hbox, "00000", 5, 5);
      break;
      
    }
  }
  
  
  void
  TimeCode::set_size_request_to_display_given_text (Gtk::Widget &w, const gchar *text,
                                                    gint hpadding, gint vpadding)
  {
    int width, height;
    //w.ensure_style();
    
    get_ink_pixel_size(w.create_pango_layout(text), width, height);
    w.set_size_request(width + hpadding, height + vpadding);
  }
  
  
  void
  TimeCode::get_ink_pixel_size (Glib::RefPtr<Pango::Layout> layout,
                                int& width, int& height)
  {
    Pango::Rectangle ink_rect = layout->get_ink_extents ();
    
    width = (ink_rect.get_width() + PANGO_SCALE / 2) / PANGO_SCALE;
    height = (ink_rect.get_height() + PANGO_SCALE / 2) / PANGO_SCALE;
  }
  
  
  
}}// gui::widget
