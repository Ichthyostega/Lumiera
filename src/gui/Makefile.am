## Process this file with automake to produce Makefile.in

SUBDIRS = src po

gtk_lumieradocdir = ${prefix}/doc/gtk-lumiera
gtk_lumieradoc_DATA = 

EXTRA_DIST = $(gtk_lumieradoc_DATA)

# Copy all the spec files. Of cource, only one is actually used.
dist-hook:
	for specfile in *.spec; do \
		if test -f $$specfile; then \
			cp -p $$specfile $(distdir); \
		fi \
	done

