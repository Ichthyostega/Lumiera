#  Copyright (C)         Lumiera.org
#    2008,               Joel Holdsworth <joel@airwebreathe.org.uk>
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

svgdir = $(top_srcdir)/icons/svg
prerendereddir = $(top_srcdir)/icons/prerendered
icondir = $(top_builddir)
iconcommand = python $(top_srcdir)/admin/render-icon.py

16x16 = $(icondir)/16x16
22x22 = $(icondir)/22x22
24x24 = $(icondir)/24x24
32x32 = $(icondir)/32x32
48x48 = $(icondir)/48x48

16x16pre = $(prerendereddir)/16x16
22x22pre = $(prerendereddir)/22x22
24x24pre = $(prerendereddir)/24x24
32x32pre = $(prerendereddir)/32x32
48x48pre = $(prerendereddir)/48x48

dist_pkgdata_DATA +=															\
	$(16x16)/app-icon.png $(22x22)/app-icon.png $(24x24)/app-icon.png $(32x32)/app-icon.png $(48x48)/app-icon.png			\
	$(16x16)/tool-arrow.png $(22x22)/tool-arrow.png $(24x24)/tool-arrow.png $(32x32)/tool-arrow.png $(48x48)/tool-arrow.png		\
	$(16x16)/tool-i-beam.png $(22x22)/tool-i-beam.png $(24x24)/tool-i-beam.png $(32x32)/tool-i-beam.png $(48x48)/tool-i-beam.png	\
	$(16x16)/track-disabled.png													\
	$(16x16)/track-enabled.png													\
	$(16x16)/track-locked.png													\
	$(16x16)/track-unlocked.png													\
	$(16x16)/panel-resources.png $(22x22)/panel-resources.png $(32x32)/panel-resources.png						\
	$(16x16)/panel-timeline.png													\
	$(16x16)/panel-viewer.png $(22x22)/panel-viewer.png $(32x32)/panel-viewer.png

clean-local:
	rm -rf $(16x16) $(22x22) $(24x24) $(32x32) $(48x48)

# ========== SVG Icons ==========

# App Icon

$(16x16)/app-icon.png $(22x22)/app-icon.png $(24x24)/app-icon.png $(32x32)/app-icon.png $(48x48)/app-icon.png : $(svgdir)/app-icon.svg $(top_builddir)/rsvg-convert
	$(iconcommand) $< $(icondir)

# Timeline Tools

$(16x16)/tool-arrow.png $(22x22)/tool-arrow.png $(24x24)/tool-arrow.png $(32x32)/tool-arrow.png $(48x48)/tool-arrow.png : $(svgdir)/tool-arrow.svg $(top_builddir)/rsvg-convert
	$(iconcommand) $< $(icondir)
$(16x16)/tool-i-beam.png $(22x22)/tool-i-beam.png $(24x24)/tool-i-beam.png $(32x32)/tool-i-beam.png $(48x48)/tool-i-beam.png : $(svgdir)/tool-i-beam.svg $(top_builddir)/rsvg-convert
	$(iconcommand) $< $(icondir)

# Timeline Tracks
$(16x16)/track-disabled.png : $(svgdir)/track-disabled.svg $(top_builddir)/rsvg-convert
	$(iconcommand) $< $(icondir)
$(16x16)/track-enabled.png : $(svgdir)/track-enabled.svg $(top_builddir)/rsvg-convert
	$(iconcommand) $< $(icondir)
$(16x16)/track-locked.png : $(svgdir)/track-locked.svg $(top_builddir)/rsvg-convert
	$(iconcommand) $< $(icondir)
$(16x16)/track-unlocked.png : $(svgdir)/track-unlocked.svg $(top_builddir)/rsvg-convert
	$(iconcommand) $< $(icondir)

# ========== Prerendered Icons ==========

# Panels

$(16x16)/panel-resources.png:
	cp $(16x16pre)/panel-resources.png $(16x16)
$(22x22)/panel-resources.png:
	cp $(22x22pre)/panel-resources.png $(22x22)
$(32x32)/panel-resources.png:
	cp $(32x32pre)/panel-resources.png $(32x32)

$(16x16)/panel-timeline.png:
	cp $(16x16pre)/panel-timeline.png $(16x16)

$(16x16)/panel-viewer.png:
	cp $(16x16pre)/panel-viewer.png $(16x16)
$(22x22)/panel-viewer.png:
	cp $(22x22pre)/panel-viewer.png $(22x22)
$(32x32)/panel-viewer.png:
	cp $(32x32pre)/panel-viewer.png $(32x32)
