#  Copyright (C)         Lumiera.org
#    2008,               Christian Thaeter <ct@pipapo.org>
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


# TODO use -Wextra -Werror

liblumiproc_a_srcdir = $(top_srcdir)/src/proc
noinst_LIBRARIES += liblumiproc.a

liblumiproc_a_CXXFLAGS = $(CXXFLAGS) -Wall
liblumiproc_a_CPPFLAGS = -I$(top_srcdir)/src/

liblumiproc_a_SOURCES =							\
	$(liblumiproc_a_srcdir)/controllerfacade.cpp			\
	$(liblumiproc_a_srcdir)/frame.cpp				\
	$(liblumiproc_a_srcdir)/stateproxy.cpp				\
	$(liblumiproc_a_srcdir)/asset.cpp				\
	$(liblumiproc_a_srcdir)/assetmanager.cpp			\
	$(liblumiproc_a_srcdir)/nobugcfg.cpp



liblumiprocasset_a_srcdir = $(top_srcdir)/src/proc/asset
noinst_LIBRARIES += liblumiprocasset.a

liblumiprocasset_a_CXXFLAGS = $(CXXFLAGS) -Wall
liblumiprocasset_a_CPPFLAGS = -I$(top_srcdir)/src/

liblumiprocasset_a_SOURCES =				\
	$(liblumiprocasset_a_srcdir)/codec.cpp		\
	$(liblumiprocasset_a_srcdir)/compoundmedia.cpp	\
	$(liblumiprocasset_a_srcdir)/dataset.cpp	\
	$(liblumiprocasset_a_srcdir)/effect.cpp		\
	$(liblumiprocasset_a_srcdir)/preview.cpp	\
	$(liblumiprocasset_a_srcdir)/unknown.cpp	\
	$(liblumiprocasset_a_srcdir)/clip.cpp		\
	$(liblumiprocasset_a_srcdir)/meta.cpp		\
	$(liblumiprocasset_a_srcdir)/category.cpp	\
	$(liblumiprocasset_a_srcdir)/media.cpp		\
	$(liblumiprocasset_a_srcdir)/pipe.cpp		\
	$(liblumiprocasset_a_srcdir)/proc.cpp		\
	$(liblumiprocasset_a_srcdir)/procpatt.cpp	\
	$(liblumiprocasset_a_srcdir)/struct.cpp		\
	$(liblumiprocasset_a_srcdir)/track.cpp



liblumiprocengine_a_srcdir = $(top_srcdir)/src/proc/engine
noinst_LIBRARIES += liblumiprocengine.a

liblumiprocengine_a_CXXFLAGS = $(CXXFLAGS) -Wall
liblumiprocengine_a_CPPFLAGS = -I$(top_srcdir)/src/

liblumiprocengine_a_SOURCES =				\
	$(liblumiprocengine_a_srcdir)/aframe.cpp	\
	$(liblumiprocengine_a_srcdir)/arender.cpp	\
	$(liblumiprocengine_a_srcdir)/codecadapter.cpp	\
	$(liblumiprocengine_a_srcdir)/exitnode.cpp	\
	$(liblumiprocengine_a_srcdir)/glbuf.cpp		\
	$(liblumiprocengine_a_srcdir)/glpipe.cpp	\
	$(liblumiprocengine_a_srcdir)/glrender.cpp	\
	$(liblumiprocengine_a_srcdir)/hub.cpp		\
	$(liblumiprocengine_a_srcdir)/link.cpp		\
	$(liblumiprocengine_a_srcdir)/mask.cpp		\
	$(liblumiprocengine_a_srcdir)/pluginadapter.cpp	\
	$(liblumiprocengine_a_srcdir)/processor.cpp	\
	$(liblumiprocengine_a_srcdir)/procnode.cpp	\
	$(liblumiprocengine_a_srcdir)/projector.cpp	\
	$(liblumiprocengine_a_srcdir)/renderengine.cpp	\
	$(liblumiprocengine_a_srcdir)/source.cpp	\
	$(liblumiprocengine_a_srcdir)/trafo.cpp		\
	$(liblumiprocengine_a_srcdir)/vframe.cpp	\
	$(liblumiprocengine_a_srcdir)/vrender.cpp



liblumiprocmobject_a_srcdir = $(top_srcdir)/src/proc/mobject
noinst_LIBRARIES += liblumiprocmobject.a

liblumiprocmobject_a_CXXFLAGS = $(CXXFLAGS) -Wall
liblumiprocmobject_a_CPPFLAGS = -I$(top_srcdir)/src/

liblumiprocmobject_a_SOURCES =					\
	$(liblumiprocmobject_a_srcdir)/builderfacade.cpp	\
	$(liblumiprocmobject_a_srcdir)/interpolator.cpp		\
	$(liblumiprocmobject_a_srcdir)/mobject.cpp		\
	$(liblumiprocmobject_a_srcdir)/parameter.cpp		\
	$(liblumiprocmobject_a_srcdir)/paramprovider.cpp	\
	$(liblumiprocmobject_a_srcdir)/placement.cpp



liblumiprocmobjectbuilder_a_srcdir = $(top_srcdir)/src/proc/mobject/builder
noinst_LIBRARIES += liblumiprocmobjectbuilder.a

liblumiprocmobjectbuilder_a_CXXFLAGS = $(CXXFLAGS) -Wall
liblumiprocmobjectbuilder_a_CPPFLAGS = -I$(top_srcdir)/src/

liblumiprocmobjectbuilder_a_SOURCES =					\
	$(liblumiprocmobjectbuilder_a_srcdir)/assembler.cpp		\
	$(liblumiprocmobjectbuilder_a_srcdir)/conmanager.cpp		\
	$(liblumiprocmobjectbuilder_a_srcdir)/nodecreatertool.cpp	\
	$(liblumiprocmobjectbuilder_a_srcdir)/segmentationtool.cpp	\
	$(liblumiprocmobjectbuilder_a_srcdir)/toolfactory.cpp


liblumiprocmobjectcontroller_a_srcdir = $(top_srcdir)/src/proc/mobject/controller
noinst_LIBRARIES += liblumiprocmobjectcontroller.a

liblumiprocmobjectcontroller_a_CXXFLAGS = $(CXXFLAGS) -Wall
liblumiprocmobjectcontroller_a_CPPFLAGS = -I$(top_srcdir)/src/

liblumiprocmobjectcontroller_a_SOURCES =				\
	$(liblumiprocmobjectcontroller_a_srcdir)/pathmanager.cpp	\
	$(liblumiprocmobjectcontroller_a_srcdir)/renderstate.cpp


liblumiprocmobjectsession_a_srcdir = $(top_srcdir)/src/proc/mobject/session
noinst_LIBRARIES += liblumiprocmobjectsession.a

liblumiprocmobjectsession_a_CXXFLAGS = $(CXXFLAGS) -Wall
liblumiprocmobjectsession_a_CPPFLAGS = -I$(top_srcdir)/src/

liblumiprocmobjectsession_a_SOURCES =					\
	$(liblumiprocmobjectsession_a_srcdir)/abstractmo.cpp		\
	$(liblumiprocmobjectsession_a_srcdir)/allocation.cpp		\
	$(liblumiprocmobjectsession_a_srcdir)/auto.cpp			\
	$(liblumiprocmobjectsession_a_srcdir)/compoundclip.cpp		\
	$(liblumiprocmobjectsession_a_srcdir)/effect.cpp		\
	$(liblumiprocmobjectsession_a_srcdir)/fixedlocation.cpp		\
	$(liblumiprocmobjectsession_a_srcdir)/label.cpp			\
	$(liblumiprocmobjectsession_a_srcdir)/meta.cpp			\
	$(liblumiprocmobjectsession_a_srcdir)/relativelocation.cpp	\
	$(liblumiprocmobjectsession_a_srcdir)/segment.cpp		\
	$(liblumiprocmobjectsession_a_srcdir)/simpleclip.cpp		\
	$(liblumiprocmobjectsession_a_srcdir)/plug.cpp			\
	$(liblumiprocmobjectsession_a_srcdir)/sessionimpl.cpp		\
	$(liblumiprocmobjectsession_a_srcdir)/sessmanagerimpl.cpp	\
	$(liblumiprocmobjectsession_a_srcdir)/edl.cpp			\
	$(liblumiprocmobjectsession_a_srcdir)/session.cpp		\
	$(liblumiprocmobjectsession_a_srcdir)/track.cpp			\
	$(liblumiprocmobjectsession_a_srcdir)/clip.cpp			\
	$(liblumiprocmobjectsession_a_srcdir)/constraint.cpp		\
	$(liblumiprocmobjectsession_a_srcdir)/defsmanager.cpp		\
	$(liblumiprocmobjectsession_a_srcdir)/fixture.cpp		\
	$(liblumiprocmobjectsession_a_srcdir)/locatingpin.cpp		\
	$(liblumiprocmobjectsession_a_srcdir)/mobjectfactory.cpp	\
	$(liblumiprocmobjectsession_a_srcdir)/wish.cpp




noinst_HEADERS +=							\
	$(liblumiproc_a_srcdir)/asset/category.hpp			\
	$(liblumiproc_a_srcdir)/asset/clip.hpp				\
	$(liblumiproc_a_srcdir)/asset/codec.hpp				\
	$(liblumiproc_a_srcdir)/asset/compoundmedia.hpp			\
	$(liblumiproc_a_srcdir)/asset/dataset.hpp			\
	$(liblumiproc_a_srcdir)/asset/effect.hpp			\
	$(liblumiproc_a_srcdir)/asset/preview.hpp			\
	$(liblumiproc_a_srcdir)/asset/db.hpp				\
	$(liblumiproc_a_srcdir)/asset/buildinstruct.hpp			\
	$(liblumiproc_a_srcdir)/asset/media.hpp				\
	$(liblumiproc_a_srcdir)/asset/meta.hpp				\
	$(liblumiproc_a_srcdir)/asset/pipe.hpp				\
	$(liblumiproc_a_srcdir)/asset/proc.hpp				\
	$(liblumiproc_a_srcdir)/asset/procpatt.hpp			\
	$(liblumiproc_a_srcdir)/asset/struct.hpp			\
	$(liblumiproc_a_srcdir)/asset/structfactoryimpl.hpp		\
	$(liblumiproc_a_srcdir)/asset/track.hpp				\
	$(liblumiproc_a_srcdir)/asset/unknown.hpp			\
	$(liblumiproc_a_srcdir)/controllerfacade.hpp			\
	$(liblumiproc_a_srcdir)/engine/aframe.hpp			\
	$(liblumiproc_a_srcdir)/engine/arender.hpp			\
	$(liblumiproc_a_srcdir)/engine/codecadapter.hpp			\
	$(liblumiproc_a_srcdir)/engine/exitnode.hpp			\
	$(liblumiproc_a_srcdir)/engine/glbuf.hpp			\
	$(liblumiproc_a_srcdir)/engine/glpipe.hpp			\
	$(liblumiproc_a_srcdir)/engine/glrender.hpp			\
	$(liblumiproc_a_srcdir)/engine/hub.hpp				\
	$(liblumiproc_a_srcdir)/engine/link.hpp				\
	$(liblumiproc_a_srcdir)/engine/mask.hpp				\
	$(liblumiproc_a_srcdir)/engine/pluginadapter.hpp		\
	$(liblumiproc_a_srcdir)/engine/procnode.hpp			\
	$(liblumiproc_a_srcdir)/engine/projector.hpp			\
	$(liblumiproc_a_srcdir)/engine/renderengine.hpp			\
	$(liblumiproc_a_srcdir)/engine/source.hpp			\
	$(liblumiproc_a_srcdir)/engine/trafo.hpp			\
	$(liblumiproc_a_srcdir)/engine/vframe.hpp			\
	$(liblumiproc_a_srcdir)/engine/vrender.hpp			\
	$(liblumiproc_a_srcdir)/engine/processor.hpp			\
	$(liblumiproc_a_srcdir)/frame.hpp				\
	$(liblumiproc_a_srcdir)/mobject/builder/assembler.hpp		\
	$(liblumiproc_a_srcdir)/mobject/builder/buildertool.hpp		\
	$(liblumiproc_a_srcdir)/mobject/builder/conmanager.hpp		\
	$(liblumiproc_a_srcdir)/mobject/builder/nodecreatertool.hpp	\
	$(liblumiproc_a_srcdir)/mobject/builder/segmentationtool.hpp	\
	$(liblumiproc_a_srcdir)/mobject/builder/toolfactory.hpp		\
	$(liblumiproc_a_srcdir)/mobject/builderfacade.hpp		\
	$(liblumiproc_a_srcdir)/mobject/controller/pathmanager.hpp	\
	$(liblumiproc_a_srcdir)/mobject/controller/renderstate.hpp	\
	$(liblumiproc_a_srcdir)/mobject/interpolator.hpp		\
	$(liblumiproc_a_srcdir)/mobject/parameter.hpp			\
	$(liblumiproc_a_srcdir)/mobject/paramprovider.hpp		\
	$(liblumiproc_a_srcdir)/mobject/session/abstractmo.hpp		\
	$(liblumiproc_a_srcdir)/mobject/session/allocation.hpp		\
	$(liblumiproc_a_srcdir)/mobject/session/auto.hpp		\
	$(liblumiproc_a_srcdir)/mobject/session/compoundclip.hpp	\
	$(liblumiproc_a_srcdir)/mobject/session/effect.hpp		\
	$(liblumiproc_a_srcdir)/mobject/session/fixedlocation.hpp	\
	$(liblumiproc_a_srcdir)/mobject/session/label.hpp		\
	$(liblumiproc_a_srcdir)/mobject/session/meta.hpp		\
	$(liblumiproc_a_srcdir)/mobject/session/relativelocation.hpp	\
	$(liblumiproc_a_srcdir)/mobject/session/simpleclip.hpp		\
	$(liblumiproc_a_srcdir)/mobject/session/mobjectfactory.hpp	\
	$(liblumiproc_a_srcdir)/mobject/session/segment.hpp		\
	$(liblumiproc_a_srcdir)/mobject/session/track.hpp		\
	$(liblumiproc_a_srcdir)/mobject/session/wish.hpp		\
	$(liblumiproc_a_srcdir)/mobject/session/clip.hpp		\
	$(liblumiproc_a_srcdir)/mobject/session/constraint.hpp		\
	$(liblumiproc_a_srcdir)/mobject/session/defsmanager.hpp		\
	$(liblumiproc_a_srcdir)/mobject/session/edl.hpp			\
	$(liblumiproc_a_srcdir)/mobject/session/fixture.hpp		\
	$(liblumiproc_a_srcdir)/mobject/session/locatingpin.hpp		\
	$(liblumiproc_a_srcdir)/mobject/session/plug.hpp		\
	$(liblumiproc_a_srcdir)/mobject/session/sessionimpl.hpp		\
	$(liblumiproc_a_srcdir)/mobject/session/defsregistry.hpp	\
	$(liblumiproc_a_srcdir)/mobject/explicitplacement.hpp		\
	$(liblumiproc_a_srcdir)/mobject/mobject.hpp			\
	$(liblumiproc_a_srcdir)/mobject/placement.hpp			\
	$(liblumiproc_a_srcdir)/mobject/session.hpp			\
	$(liblumiproc_a_srcdir)/stateproxy.hpp				\
	$(liblumiproc_a_srcdir)/asset.hpp				\
	$(liblumiproc_a_srcdir)/assetmanager.hpp			\
	$(liblumiproc_a_srcdir)/lumiera.hpp				\
	$(liblumiproc_a_srcdir)/nobugcfg.hpp

