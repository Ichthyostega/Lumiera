## Process this file with automake to produce Makefile.in

INCLUDES = \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	$(GTK_LUMIERA_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g

bin_PROGRAMS = gtk-lumiera

gtk_lumiera_SOURCES =                 \
	  gtk-lumiera.cpp                   \
    gtk-lumiera.hpp                   \
	  window-manager.cpp                \
    window-manager.hpp                \
    workspace/actions.cpp             \
    workspace/actions.hpp             \
    workspace/workspace-window.cpp    \
    workspace/workspace-window.hpp    \
    dialogs/render.cpp                \
    dialogs/render.hpp                \
    dialogs/preferences-dialog.cpp    \
    dialogs/preferences-dialog.hpp    \
    panels/panel.cpp                  \
    panels/panel.hpp                  \
    panels/timeline-panel.cpp         \
    panels/timeline-panel.hpp         \
    panels/viewer-panel.cpp           \
    panels/viewer-panel.hpp           \
    panels/assets-panel.cpp           \
    panels/asset-panels.hpp           \
    widgets/video-display-widget.cpp  \
    widgets/video-display-widget.hpp  \
    widgets/timeline-widget.cpp       \
    widgets/timeline-widget.hpp       \
    model/project.cpp                 \
    model/project.hpp                 \
    output/displayer.cpp              \
    output/displayer.hpp              \
    output/xvdisplayer.cpp            \
    output/xvdisplayer.hpp            

gtk_lumiera_LDFLAGS = 

gtk_lumiera_LDADD = $(GTK_LUMIERA_LIBS)

EXTRA_DIST = 


