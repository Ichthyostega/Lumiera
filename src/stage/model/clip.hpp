/*
  clip.hpp  -  Definition of the Clip class

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

/** @file gui/model/clip.hpp
 ** Preliminary UI-model: a Proxy Clip object to base the GUI implementation on.
 ** Later this Clip object will be connected to the underlying model in Steam-Layer.
 ** @warning as of 2016 this UI model is known to be a temporary workaround
 **          and will be replaced in entirety by UI-Bus and diff framework.
 */

#ifndef STAGE_MODEL_CLIP_H
#define STAGE_MODEL_CLIP_H

#include "stage/gtk-base.hpp"
#include "lib/time/timevalue.hpp"

#include <string>


namespace gui {
namespace model {
  
  using std::string;
  using lib::time::Time;
  using lib::time::TimeSpan;
  using lib::time::Duration;
  using Cairo::Pattern;
  
  
  class Clip
    {
      TimeSpan timeCoord_;
      string name_;
  
      /** fires when the name changes. */
      sigc::signal<void, string> nameChangedSignal_;
      
      
    public:
        
      Clip();
  
      Time getBegin()  const { return timeCoord_.start();}
      Time getEnd()    const { return timeCoord_.end();  }
  
      string const& getName() const { return name_; }
      
      
      /**
       * Check whether or not the clip will be playing during the given time.
       */
      bool
      isPlayingAt (Time const& position) const
        {
          return timeCoord_.contains (position);
        }

  
      /**
       * Sets the begin time of this clip.
       * @param[in] begin The new begin time to set this clip to.
       */
      void setBegin (Time);
  
      /**
       * Sets the end time of this clip.
       * @param[in] end The new end time to set this clip to.
       */
      void setDuration (Duration);
  
      /**
       * Sets the name of this clip.
       * @param[in] name The new name to set this clip to.
       */
      void setName (string const&);
  
      /**
       * A signal which fires when the name changes.
       * @return Returns the signal. The signal sends the new name for the clip.
       */
      sigc::signal<void, std::string>
      signalNameChanged() const;
  
    };
  
  
}}   // namespace gui::model
#endif // CLIP_HPP
