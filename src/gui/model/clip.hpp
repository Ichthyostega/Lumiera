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
/** @file model/clip.hpp
 ** This file contains the definition of the Clip object
 */

#include <string>
#include "gui/gtk-lumiera.hpp"

#include "lib/lumitime.hpp"

// TODO: Remove once we get real measure of duration.
//       This is here *only* for purposes of testing the GUI.
extern "C" {
#include <stdint.h>
#include <gavl/gavltime.h>
}

using Cairo::Pattern;

#ifndef CLIP_HPP
#define CLIP_HPP

namespace gui {
namespace model {

  class Clip
  {
  public:
    /**
     * Constructor
     **/
    Clip();

    /**
     * Gets the begin time of this clip.
     **/
    gavl_time_t
    getBegin() const;

    /**
     * Gets the end time of this clip.
     **/
    gavl_time_t
    getEnd() const;

    /**
     * Gets the name of this clip.
     **/
    const std::string
    getName() const;

    /**
     * Check whether or not the clip will be playing during the given time.
     **/
    bool
    isPlayingAt(lumiera::Time position) const;

    /**
     * Sets the begin time of this clip.
     * @param[in] begin The new begin time to set this clip to.
     **/
    void
    setBegin(gavl_time_t begin);

    /**
     * Sets the end time of this clip.
     * @param[in] end The new end time to set this clip to.
     **/
    void
    setEnd(gavl_time_t end);

    /**
     * Sets the name of this clip.
     * @param[in] name The new name to set this clip to.
     **/
    void
    setName(const std::string &name);

    /**
     * A signal which fires when the name changes.
     * @return Returns the signal. The signal sends the new name for the clip.
     **/
    sigc::signal<void, std::string>
    signalNameChanged() const;

  private:

    /**
     * The name of this clip.
     **/
    std::string name;

    /**
     * A signal which fires when the name changes.
     **/
    sigc::signal<void, std::string> nameChangedSignal;

    // TODO: Use a good measure of duration, probably TimeSpan.
    //       These are here *only* for purposes of testing the GUI.
    gavl_time_t begin;
    gavl_time_t end;
  };

}   // namespace model
}   // namespace gui

#endif // CLIP_HPP
