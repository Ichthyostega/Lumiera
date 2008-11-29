#  Copyright (C)		 Lumiera.org
#	2007,			   Joel Holdsworth <joel@airwebreathe.org.uk>
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

lumigui_CPPFLAGS = $(AM_CPPFLAGS) \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\"				\
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\"										\
	-DPACKAGE_DATA_DIR=\""$(datadir)"\"										\
	$(LUMIERA_GUI_CFLAGS)

bin_PROGRAMS += lumigui

lumigui_SOURCES =															\
	$(lumigui_srcdir)/gtk-lumiera.cpp										\
	$(lumigui_srcdir)/gtk-lumiera.hpp										\
	$(lumigui_srcdir)/window-manager.cpp									\
	$(lumigui_srcdir)/window-manager.hpp									\
	$(lumigui_srcdir)/workspace/actions.cpp									\
	$(lumigui_srcdir)/workspace/actions.hpp									\
	$(lumigui_srcdir)/workspace/workspace-window.cpp						\
	$(lumigui_srcdir)/workspace/workspace-window.hpp						\
	$(lumigui_srcdir)/dialogs/dialog.hpp									\
	$(lumigui_srcdir)/dialogs/render.cpp									\
	$(lumigui_srcdir)/dialogs/render.hpp									\
	$(lumigui_srcdir)/dialogs/preferences-dialog.cpp						\
	$(lumigui_srcdir)/dialogs/preferences-dialog.hpp						\
	$(lumigui_srcdir)/dialogs/sequence-name.cpp								\
	$(lumigui_srcdir)/dialogs/sequence-name.hpp								\
	$(lumigui_srcdir)/panels/panel.cpp										\
	$(lumigui_srcdir)/panels/panel.hpp										\
	$(lumigui_srcdir)/panels/timeline-panel.cpp								\
	$(lumigui_srcdir)/panels/timeline-panel.hpp								\
	$(lumigui_srcdir)/panels/viewer-panel.cpp								\
	$(lumigui_srcdir)/panels/viewer-panel.hpp								\
	$(lumigui_srcdir)/panels/assets-panel.cpp								\
	$(lumigui_srcdir)/panels/assets-panel.hpp								\
	$(lumigui_srcdir)/widgets/video-display-widget.cpp						\
	$(lumigui_srcdir)/widgets/video-display-widget.hpp						\
	$(lumigui_srcdir)/widgets/timeline-widget.cpp							\
	$(lumigui_srcdir)/widgets/timeline-widget.hpp							\
	$(lumigui_srcdir)/widgets/timeline/timeline-view-window.cpp				\
	$(lumigui_srcdir)/widgets/timeline/timeline-view-window.hpp				\
	$(lumigui_srcdir)/widgets/timeline/timeline-header-container.cpp		\
	$(lumigui_srcdir)/widgets/timeline/timeline-header-container.hpp		\
	$(lumigui_srcdir)/widgets/timeline/timeline-body.cpp					\
	$(lumigui_srcdir)/widgets/timeline/timeline-body.hpp					\
	$(lumigui_srcdir)/widgets/timeline/timeline-ruler.cpp					\
	$(lumigui_srcdir)/widgets/timeline/timeline-ruler.hpp					\
	$(lumigui_srcdir)/widgets/timeline/timeline-tool.cpp					\
	$(lumigui_srcdir)/widgets/timeline/timeline-tool.hpp					\
	$(lumigui_srcdir)/widgets/timeline/timeline-arrow-tool.cpp				\
	$(lumigui_srcdir)/widgets/timeline/timeline-arrow-tool.hpp				\
	$(lumigui_srcdir)/widgets/timeline/timeline-ibeam-tool.cpp				\
	$(lumigui_srcdir)/widgets/timeline/timeline-ibeam-tool.hpp				\
	$(lumigui_srcdir)/widgets/timeline/timeline-track.cpp					\
	$(lumigui_srcdir)/widgets/timeline/timeline-track.hpp					\
	$(lumigui_srcdir)/widgets/timeline/timeline-clip-track.cpp				\
	$(lumigui_srcdir)/widgets/timeline/timeline-clip-track.hpp				\
	$(lumigui_srcdir)/widgets/timeline/timeline-group-track.cpp				\
	$(lumigui_srcdir)/widgets/timeline/timeline-group-track.hpp				\
	$(lumigui_srcdir)/widgets/timeline/timeline-clip.cpp					\
	$(lumigui_srcdir)/widgets/timeline/timeline-clip.hpp					\
	$(lumigui_srcdir)/model/project.cpp										\
	$(lumigui_srcdir)/model/project.hpp										\
	$(lumigui_srcdir)/model/sequence.cpp									\
	$(lumigui_srcdir)/model/sequence.hpp									\
	$(lumigui_srcdir)/model/track.cpp										\
	$(lumigui_srcdir)/model/track.hpp										\
	$(lumigui_srcdir)/model/clip-track.cpp									\
	$(lumigui_srcdir)/model/clip-track.hpp									\
	$(lumigui_srcdir)/model/group-track.cpp									\
	$(lumigui_srcdir)/model/group-track.hpp									\
	$(lumigui_srcdir)/model/clip.cpp										\
	$(lumigui_srcdir)/model/clip.hpp										\
	$(lumigui_srcdir)/output/displayer.cpp									\
	$(lumigui_srcdir)/output/displayer.hpp									\
	$(lumigui_srcdir)/output/gdkdisplayer.cpp								\
	$(lumigui_srcdir)/output/gdkdisplayer.hpp								\
	$(lumigui_srcdir)/output/xvdisplayer.cpp								\
	$(lumigui_srcdir)/output/xvdisplayer.hpp

lumigui_LDFLAGS =
# $(LIBS) $(X_PRE_LIBS) $(X_LIBS) $(X_EXTRA_LIBS)
lumigui_LDADD = $(LUMIERA_GUI_LIBS) liblumicommon.a liblumiera.a $(NOBUGMT_LUMIERA_LIBS)

lumigui_DEPENDENCIES =														\
	$(top_builddir)/lumiera_ui.rc											\
	$(top_builddir)/liblumicommon.a											\
	$(top_builddir)/liblumiera.a

$(top_builddir)/lumiera_ui.rc:
	cp $(lumigui_srcdir)/lumiera_ui.rc $(top_builddir)

