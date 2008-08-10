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

lumigui_DEPENDENCIES += \
    $(16x16)/arrow.png $(22x22)/arrow.png $(24x24)/arrow.png $(32x32)/arrow.png $(48x48)/arrow.png \
    $(16x16)/i-beam.png $(22x22)/i-beam.png $(24x24)/i-beam.png $(32x32)/i-beam.png $(48x48)/i-beam.png \
    $(16x16)/assets-panel.png \
    $(16x16)/timeline-panel.png \
    $(16x16)/viewer-panel.png  

$(16x16)/arrow.png $(22x22)/arrow.png $(24x24)/arrow.png $(32x32)/arrow.png $(48x48)/arrow.png : $(svgdir)/arrow.svg
	$(iconcommand) $< $(icondir)
$(16x16)/i-beam.png $(22x22)/i-beam.png $(24x24)/i-beam.png $(32x32)/i-beam.png $(48x48)/i-beam.png : $(svgdir)/i-beam.svg
	$(iconcommand) $< $(icondir)

$(16x16)/assets-panel.png:
	cp $(16x16pre)/assets-panel.png $(16x16)
$(16x16)/timeline-panel.png:
	cp $(16x16pre)/timeline-panel.png $(16x16)
$(16x16)/viewer-panel.png:
	cp $(16x16pre)/viewer-panel.png $(16x16)

