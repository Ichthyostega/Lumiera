## Process this file with automake to produce Makefile.in

gladedir = $(datadir)/gtk-lumiera/glade
glade_DATA = gtk-lumiera.glade

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
    workspace/actions.cpp             \
    workspace/actions.hpp             \
    workspace/mainwindow.cpp          \
    workspace/mainwindow.hpp          \
    dialogs/render.cpp                \
    dialogs/render.hpp                 

gtk_lumiera_LDFLAGS = 

gtk_lumiera_LDADD = $(GTK_LUMIERA_LIBS)

EXTRA_DIST = $(glade_DATA)


