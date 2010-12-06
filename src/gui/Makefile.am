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

#
# themes, locales and other supplemental data
#
dist_pkgdata_DATA += lumiera_ui.rc
CLEANFILES += lumiera_ui.rc

lumiera_ui.rc:
	cp $(lumigui_srcdir)/lumiera_ui.rc $(top_builddir)


#
# gtk_gui.lum becomes the pluginized gui
#
pkglib_LTLIBRARIES += gtk_gui.la

gtk_gui_la_CFLAGS = $(AM_CFLAGS) $(LUMIERA_GUI_CFLAGS) -std=gnu99 -Wall -Wextra -Werror
gtk_gui_la_CXXFLAGS = $(AM_CXXFLAGS) $(LUMIERA_GUI_CFLAGS) -Wall -Wextra
gtk_gui_la_CPPFLAGS = $(AM_CPPFLAGS)					\
	$(LUMIERA_GUI_CFLAGS)						\
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\"	\
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\"				\
	-DPACKAGE_DATA_DIR=\""$(datadir)"\"				\
	-DLUMIERA_PLUGIN -I$(top_srcdir)/src/

gtk_gui_la_LDFLAGS = -module -avoid-version -no-undefined -rpath /dev/null -shrext .lum

gtk_gui_la_LIBADD =		\
	$(LUMIERA_GUI_LIBS)	\
	liblumiera.la		\
	liblumierabackend.la	\
	liblumieracommon.la	\
	liblumieraproc.la

gtk_gui_la_SOURCES =								\
	$(lumigui_srcdir)/controller/controller.cpp				\
	$(lumigui_srcdir)/controller/controller.hpp				\
	$(lumigui_srcdir)/controller/playback-controller.cpp			\
	$(lumigui_srcdir)/controller/playback-controller.hpp			\
	$(lumigui_srcdir)/dialogs/dialog.hpp					\
	$(lumigui_srcdir)/dialogs/name-chooser.cpp				\
	$(lumigui_srcdir)/dialogs/name-chooser.hpp				\
	$(lumigui_srcdir)/dialogs/preferences-dialog.cpp			\
	$(lumigui_srcdir)/dialogs/preferences-dialog.hpp			\
	$(lumigui_srcdir)/dialogs/render.cpp					\
	$(lumigui_srcdir)/dialogs/render.hpp					\
	$(lumigui_srcdir)/display-service.cpp					\
	$(lumigui_srcdir)/gtk-lumiera.cpp					\
	$(lumigui_srcdir)/gtk-lumiera.hpp					\
	$(lumigui_srcdir)/guistart.cpp						\
	$(lumigui_srcdir)/model/clip-track.cpp					\
	$(lumigui_srcdir)/model/clip-track.hpp					\
	$(lumigui_srcdir)/model/clip.cpp					\
	$(lumigui_srcdir)/model/clip.hpp					\
	$(lumigui_srcdir)/model/group-track.cpp					\
	$(lumigui_srcdir)/model/group-track.hpp					\
	$(lumigui_srcdir)/model/parent-track.cpp				\
	$(lumigui_srcdir)/model/parent-track.hpp				\
	$(lumigui_srcdir)/model/project.cpp					\
	$(lumigui_srcdir)/model/project.hpp					\
	$(lumigui_srcdir)/model/sequence.cpp					\
	$(lumigui_srcdir)/model/sequence.hpp					\
	$(lumigui_srcdir)/model/track.cpp					\
	$(lumigui_srcdir)/model/track.hpp					\
	$(lumigui_srcdir)/notification-service.cpp				\
	$(lumigui_srcdir)/output/displayer.cpp					\
	$(lumigui_srcdir)/output/displayer.hpp					\
	$(lumigui_srcdir)/output/gdkdisplayer.cpp				\
	$(lumigui_srcdir)/output/gdkdisplayer.hpp				\
	$(lumigui_srcdir)/output/xvdisplayer.cpp				\
	$(lumigui_srcdir)/output/xvdisplayer.hpp				\
	$(lumigui_srcdir)/panels/panel.cpp					\
	$(lumigui_srcdir)/panels/panel.hpp					\
	$(lumigui_srcdir)/panels/assets-panel.cpp				\
	$(lumigui_srcdir)/panels/assets-panel.hpp				\
	$(lumigui_srcdir)/panels/timeline-panel.cpp				\
	$(lumigui_srcdir)/panels/timeline-panel.hpp				\
	$(lumigui_srcdir)/panels/viewer-panel.cpp				\
	$(lumigui_srcdir)/panels/viewer-panel.hpp				\
	$(lumigui_srcdir)/util/cairo-util.cpp					\
	$(lumigui_srcdir)/util/cairo-util.hpp					\
	$(lumigui_srcdir)/util/convert.cpp					\
	$(lumigui_srcdir)/util/convert.hpp					\
	$(lumigui_srcdir)/util/rectangle.cpp					\
	$(lumigui_srcdir)/util/rectangle.hpp					\
	$(lumigui_srcdir)/widgets/button-bar.cpp				\
	$(lumigui_srcdir)/widgets/button-bar.hpp				\
	$(lumigui_srcdir)/widgets/menu-button.cpp				\
	$(lumigui_srcdir)/widgets/menu-button.hpp				\
	$(lumigui_srcdir)/widgets/mini-button.hpp				\
	$(lumigui_srcdir)/widgets/panel-bar.cpp					\
	$(lumigui_srcdir)/widgets/panel-bar.hpp					\
	$(lumigui_srcdir)/widgets/timecode-widget.cpp				\
	$(lumigui_srcdir)/widgets/timecode-widget.hpp				\
	$(lumigui_srcdir)/widgets/timeline-widget.cpp				\
	$(lumigui_srcdir)/widgets/timeline-widget.hpp				\
	$(lumigui_srcdir)/widgets/timeline/timeline-arrow-tool.cpp		\
	$(lumigui_srcdir)/widgets/timeline/timeline-arrow-tool.hpp		\
	$(lumigui_srcdir)/widgets/timeline/timeline-body.cpp			\
	$(lumigui_srcdir)/widgets/timeline/timeline-body.hpp			\
	$(lumigui_srcdir)/widgets/timeline/timeline-clip-track.cpp		\
	$(lumigui_srcdir)/widgets/timeline/timeline-clip-track.hpp		\
	$(lumigui_srcdir)/widgets/timeline/timeline-clip.cpp			\
	$(lumigui_srcdir)/widgets/timeline/timeline-clip.hpp			\
	$(lumigui_srcdir)/widgets/timeline/timeline-group-track.cpp		\
	$(lumigui_srcdir)/widgets/timeline/timeline-group-track.hpp		\
	$(lumigui_srcdir)/widgets/timeline/timeline-header-container.cpp	\
	$(lumigui_srcdir)/widgets/timeline/timeline-header-container.hpp	\
	$(lumigui_srcdir)/widgets/timeline/timeline-header-widget.cpp		\
	$(lumigui_srcdir)/widgets/timeline/timeline-header-widget.hpp		\
	$(lumigui_srcdir)/widgets/timeline/timeline-ibeam-tool.cpp		\
	$(lumigui_srcdir)/widgets/timeline/timeline-ibeam-tool.hpp		\
	$(lumigui_srcdir)/widgets/timeline/timeline-layout-helper.cpp		\
	$(lumigui_srcdir)/widgets/timeline/timeline-layout-helper.hpp		\
	$(lumigui_srcdir)/widgets/timeline/timeline-ruler.cpp			\
	$(lumigui_srcdir)/widgets/timeline/timeline-ruler.hpp			\
	$(lumigui_srcdir)/widgets/timeline/timeline-state.cpp			\
	$(lumigui_srcdir)/widgets/timeline/timeline-state.hpp			\
	$(lumigui_srcdir)/widgets/timeline/timeline-tool.cpp			\
	$(lumigui_srcdir)/widgets/timeline/timeline-tool.hpp			\
	$(lumigui_srcdir)/widgets/timeline/timeline-track.cpp			\
	$(lumigui_srcdir)/widgets/timeline/timeline-track.hpp			\
	$(lumigui_srcdir)/widgets/timeline/timeline-view-window.cpp		\
	$(lumigui_srcdir)/widgets/timeline/timeline-view-window.hpp		\
	$(lumigui_srcdir)/widgets/video-display-widget.cpp			\
	$(lumigui_srcdir)/widgets/video-display-widget.hpp			\
	$(lumigui_srcdir)/window-manager.cpp					\
	$(lumigui_srcdir)/window-manager.hpp					\
	$(lumigui_srcdir)/workspace/actions.cpp					\
	$(lumigui_srcdir)/workspace/actions.hpp					\
	$(lumigui_srcdir)/workspace/panel-manager.cpp				\
	$(lumigui_srcdir)/workspace/panel-manager.hpp				\
	$(lumigui_srcdir)/workspace/workspace-window.cpp			\
	$(lumigui_srcdir)/workspace/workspace-window.hpp





