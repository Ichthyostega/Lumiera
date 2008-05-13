#  Copyright (C)         Lumiera.org
#    2007,               Joel Holdsworth <joel@airwebreathe.org.uk>
#
#  This program is free software; you can redistribute it and/or
#  modify it under the terms of the GNU General Public License as
#  published by the Free Software Foundation; either version 2 of the
#  License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

lumigui_srcdir = $(top_srcdir)/src/gui
#noinst_LIBRARIES += liblumigui.a

#lumigui_CFLAGS = $(CFLAGS) -std=gnu99 -Wall -Werror
#lumigui_CPPFLAGS = -I$(top_srcdir)/src/

INCLUDES = \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	$(GTK_LUMIERA_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g

bin_PROGRAMS += lumigui

lumigui_SOURCES =                                            \
	  $(lumigui_srcdir)/gtk-lumiera.cpp                   \
    $(lumigui_srcdir)/gtk-lumiera.hpp                   \
	  $(lumigui_srcdir)/window-manager.cpp                \
    $(lumigui_srcdir)/window-manager.hpp                \
    $(lumigui_srcdir)/workspace/actions.cpp             \
    $(lumigui_srcdir)/workspace/actions.hpp             \
    $(lumigui_srcdir)/workspace/workspace-window.cpp    \
    $(lumigui_srcdir)/workspace/workspace-window.hpp    \
    $(lumigui_srcdir)/dialogs/render.cpp                \
    $(lumigui_srcdir)/dialogs/render.hpp                \
    $(lumigui_srcdir)/dialogs/preferences-dialog.cpp    \
    $(lumigui_srcdir)/dialogs/preferences-dialog.hpp    \
    $(lumigui_srcdir)/panels/panel.cpp                  \
    $(lumigui_srcdir)/panels/panel.hpp                  \
    $(lumigui_srcdir)/panels/timeline-panel.cpp         \
    $(lumigui_srcdir)/panels/timeline-panel.hpp         \
    $(lumigui_srcdir)/panels/viewer-panel.cpp           \
    $(lumigui_srcdir)/panels/viewer-panel.hpp           \
    $(lumigui_srcdir)/panels/assets-panel.cpp           \
    $(lumigui_srcdir)/panels/asset-panels.hpp           \
    $(lumigui_srcdir)/widgets/video-display-widget.cpp  \
    $(lumigui_srcdir)/widgets/video-display-widget.hpp  \
    $(lumigui_srcdir)/widgets/timeline-widget.cpp       \
    $(lumigui_srcdir)/widgets/timeline-widget.hpp       \
    $(lumigui_srcdir)/model/project.cpp                 \
    $(lumigui_srcdir)/model/project.hpp                 \
    $(lumigui_srcdir)/output/displayer.cpp              \
    $(lumigui_srcdir)/output/displayer.hpp              \
    $(lumigui_srcdir)/output/gdkdisplayer.cpp           \
    $(lumigui_srcdir)/output/gdkdisplayer.hpp           \
    $(lumigui_srcdir)/output/xvdisplayer.cpp            \
    $(lumigui_srcdir)/output/xvdisplayer.hpp

lumigui_LDFLAGS = 
lumigui_LDADD = $(GTK_LUMIERA_LIBS) 

themesdir = $(datadir)/themes
themes_DATA = $(lumigui_srcdir)/lumiera_ui.rc

EXTRA_DIST += $(themes_DATA)
