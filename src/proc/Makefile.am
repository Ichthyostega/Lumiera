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

liblumiproc_la_srcdir = $(top_srcdir)/src/proc
noinst_LTLIBRARIES += liblumiproc.la

liblumiproc_la_CFLAGS = $(AM_CFLAGS) $(LUMIERA_PROC_CFLAGS) -std=gnu99 -Wall -Wextra -Werror
liblumiproc_la_CXXFLAGS = $(AM_CXXFLAGS) $(LUMIERA_PROC_CFLAGS) -Wall -Wextra

liblumiproc_la_SOURCES =				\
	$(liblumiproc_la_srcdir)/asset.cpp		\
	$(liblumiproc_la_srcdir)/assetmanager.cpp	\
	$(liblumiproc_la_srcdir)/controllerfacade.cpp	\
	$(liblumiproc_la_srcdir)/facade.cpp		\
	$(liblumiproc_la_srcdir)/state.cpp



liblumiprocasset_la_srcdir = $(top_srcdir)/src/proc/asset
noinst_LTLIBRARIES += liblumiprocasset.la

liblumiprocasset_la_CFLAGS = $(AM_CFLAGS) $(LUMIERA_PROC_CFLAGS) -std=gnu99 -Wall -Wextra -Werror
liblumiprocasset_la_CXXFLAGS = $(AM_CXXFLAGS) $(LUMIERA_PROC_CFLAGS) -Wall -Wextra

liblumiprocasset_la_SOURCES =					\
	$(liblumiprocasset_la_srcdir)/category.cpp		\
	$(liblumiprocasset_la_srcdir)/clip.cpp			\
	$(liblumiprocasset_la_srcdir)/codec.cpp			\
	$(liblumiprocasset_la_srcdir)/compoundmedia.cpp		\
	$(liblumiprocasset_la_srcdir)/effect.cpp		\
	$(liblumiprocasset_la_srcdir)/media.cpp			\
	$(liblumiprocasset_la_srcdir)/meta.cpp			\
	$(liblumiprocasset_la_srcdir)/meta/time-grid.cpp	\
	$(liblumiprocasset_la_srcdir)/pipe.cpp			\
	$(liblumiprocasset_la_srcdir)/preview.cpp		\
	$(liblumiprocasset_la_srcdir)/proc.cpp			\
	$(liblumiprocasset_la_srcdir)/procpatt.cpp		\
	$(liblumiprocasset_la_srcdir)/sequence.cpp		\
	$(liblumiprocasset_la_srcdir)/struct.cpp		\
	$(liblumiprocasset_la_srcdir)/timeline.cpp		\
	$(liblumiprocasset_la_srcdir)/typed-lookup.cpp		\
	$(liblumiprocasset_la_srcdir)/unknown.cpp


liblumiprocengine_la_srcdir = $(top_srcdir)/src/proc/engine
noinst_LTLIBRARIES += liblumiprocengine.la

liblumiprocengine_la_CFLAGS = $(AM_CFLAGS) $(LUMIERA_PROC_CFLAGS) -std=gnu99 -Wall -Wextra -Werror
liblumiprocengine_la_CXXFLAGS = $(AM_CXXFLAGS) $(LUMIERA_PROC_CFLAGS) -Wall -Wextra

liblumiprocengine_la_SOURCES =					\
	$(liblumiprocengine_la_srcdir)/buffhandle.cpp		\
	$(liblumiprocengine_la_srcdir)/link.cpp			\
	$(liblumiprocengine_la_srcdir)/mask.cpp			\
	$(liblumiprocengine_la_srcdir)/nodefactory.cpp		\
	$(liblumiprocengine_la_srcdir)/nodewiring.cpp		\
	$(liblumiprocengine_la_srcdir)/pluginadapter.cpp	\
	$(liblumiprocengine_la_srcdir)/projector.cpp		\
	$(liblumiprocengine_la_srcdir)/render-invocation.cpp	\
	$(liblumiprocengine_la_srcdir)/renderengine.cpp		\
	$(liblumiprocengine_la_srcdir)/rendergraph.cpp		\
	$(liblumiprocengine_la_srcdir)/source.cpp		\
	$(liblumiprocengine_la_srcdir)/stateproxy.cpp		\
	$(liblumiprocengine_la_srcdir)/trafo.cpp



liblumiprocmobject_la_srcdir = $(top_srcdir)/src/proc/mobject
noinst_LTLIBRARIES += liblumiprocmobject.la

liblumiprocmobject_la_CFLAGS = $(AM_CFLAGS) $(LUMIERA_PROC_CFLAGS) -std=gnu99 -Wall -Wextra -Werror
liblumiprocmobject_la_CXXFLAGS = $(AM_CXXFLAGS) $(LUMIERA_PROC_CFLAGS) -Wall -Wextra

liblumiprocmobject_la_SOURCES =					\
	$(liblumiprocmobject_la_srcdir)/builderfacade.cpp	\
	$(liblumiprocmobject_la_srcdir)/interpolator.cpp	\
	$(liblumiprocmobject_la_srcdir)/mobject-ref.cpp		\
	$(liblumiprocmobject_la_srcdir)/mobject.cpp		\
	$(liblumiprocmobject_la_srcdir)/output-designation.cpp	\
	$(liblumiprocmobject_la_srcdir)/parameter.cpp		\
	$(liblumiprocmobject_la_srcdir)/paramprovider.cpp	\
	$(liblumiprocmobject_la_srcdir)/placement.cpp



liblumiprocmobjectbuilder_la_srcdir = $(top_srcdir)/src/proc/mobject/builder
noinst_LTLIBRARIES += liblumiprocmobjectbuilder.la

liblumiprocmobjectbuilder_la_CFLAGS = $(AM_CFLAGS) $(LUMIERA_PROC_CFLAGS) -std=gnu99 -Wall -Wextra -Werror
liblumiprocmobjectbuilder_la_CXXFLAGS = $(AM_CXXFLAGS) $(LUMIERA_PROC_CFLAGS) -Wall -Wextra

liblumiprocmobjectbuilder_la_SOURCES =					\
	$(liblumiprocmobjectbuilder_la_srcdir)/assembler.cpp		\
	$(liblumiprocmobjectbuilder_la_srcdir)/conmanager.cpp		\
	$(liblumiprocmobjectbuilder_la_srcdir)/model-port-registry.cpp	\
	$(liblumiprocmobjectbuilder_la_srcdir)/operationpoint.cpp	\
	$(liblumiprocmobjectbuilder_la_srcdir)/nodecreatortool.cpp	\
	$(liblumiprocmobjectbuilder_la_srcdir)/segmentation-tool.cpp	\
	$(liblumiprocmobjectbuilder_la_srcdir)/segmentation.cpp		\
	$(liblumiprocmobjectbuilder_la_srcdir)/toolfactory.cpp


liblumiproccontrol_la_srcdir = $(top_srcdir)/src/proc/control
noinst_LTLIBRARIES += liblumiproccontrol.la

liblumiproccontrol_la_CFLAGS = $(AM_CFLAGS) $(LUMIERA_PROC_CFLAGS) -std=gnu99 -Wall -Wextra -Werror
liblumiproccontrol_la_CXXFLAGS = $(AM_CXXFLAGS) $(LUMIERA_PROC_CFLAGS) -Wall -Wextra

liblumiproccontrol_la_SOURCES =					\
	$(liblumiproccontrol_la_srcdir)/command-impl.cpp	\
	$(liblumiproccontrol_la_srcdir)/command.cpp		\
	$(liblumiproccontrol_la_srcdir)/handling-pattern.cpp	\
	$(liblumiproccontrol_la_srcdir)/pathmanager.cpp		\
	$(liblumiproccontrol_la_srcdir)/proc-dispatcher.cpp	\
	$(liblumiproccontrol_la_srcdir)/stypemanager.cpp


liblumiprocplay_la_srcdir = $(top_srcdir)/src/proc/play
noinst_LTLIBRARIES += liblumiprocplay.la

liblumiprocplay_la_CFLAGS = $(AM_CFLAGS) $(LUMIERA_PROC_CFLAGS) -std=gnu99 -Wall -Wextra -Werror
liblumiprocplay_la_CXXFLAGS = $(AM_CXXFLAGS) $(LUMIERA_PROC_CFLAGS) -Wall -Wextra

liblumiprocplay_la_SOURCES =					\
	$(liblumiprocplay_la_srcdir)/dummy-image-generator.cpp	\
	$(liblumiprocplay_la_srcdir)/dummy-player-service.cpp


liblumiprocmobjectsession_la_srcdir = $(top_srcdir)/src/proc/mobject/session
noinst_LTLIBRARIES += liblumiprocmobjectsession.la

liblumiprocmobjectsession_la_CFLAGS = $(AM_CFLAGS) $(LUMIERA_PROC_CFLAGS) -std=gnu99 -Wall -Wextra -Werror
liblumiprocmobjectsession_la_CXXFLAGS = $(AM_CXXFLAGS) $(LUMIERA_PROC_CFLAGS) -Wall -Wextra

liblumiprocmobjectsession_la_SOURCES =							\
	$(liblumiprocmobjectsession_la_srcdir)/abstractmo.cpp				\
	$(liblumiprocmobjectsession_la_srcdir)/allocation.cpp				\
	$(liblumiprocmobjectsession_la_srcdir)/auto.cpp					\
	$(liblumiprocmobjectsession_la_srcdir)/binding.cpp				\
	$(liblumiprocmobjectsession_la_srcdir)/bus-mo.cpp				\
	$(liblumiprocmobjectsession_la_srcdir)/clip.cpp					\
	$(liblumiprocmobjectsession_la_srcdir)/compoundclip.cpp				\
	$(liblumiprocmobjectsession_la_srcdir)/constraint.cpp				\
	$(liblumiprocmobjectsession_la_srcdir)/defs-manager.cpp				\
	$(liblumiprocmobjectsession_la_srcdir)/effect.cpp				\
	$(liblumiprocmobjectsession_la_srcdir)/fixedlocation.cpp			\
	$(liblumiprocmobjectsession_la_srcdir)/fixture.cpp				\
	$(liblumiprocmobjectsession_la_srcdir)/label.cpp				\
	$(liblumiprocmobjectsession_la_srcdir)/locatingpin.cpp				\
	$(liblumiprocmobjectsession_la_srcdir)/meta.cpp					\
	$(liblumiprocmobjectsession_la_srcdir)/mobjectfactory.cpp			\
	$(liblumiprocmobjectsession_la_srcdir)/placement-index-query-resolver.cpp 	\
	$(liblumiprocmobjectsession_la_srcdir)/placement-index.cpp			\
	$(liblumiprocmobjectsession_la_srcdir)/plug.cpp					\
	$(liblumiprocmobjectsession_la_srcdir)/query-focus.cpp 				\
	$(liblumiprocmobjectsession_la_srcdir)/query-resolver.cpp 			\
	$(liblumiprocmobjectsession_la_srcdir)/relativelocation.cpp			\
	$(liblumiprocmobjectsession_la_srcdir)/root.cpp 				\
	$(liblumiprocmobjectsession_la_srcdir)/scope-path.cpp 				\
	$(liblumiprocmobjectsession_la_srcdir)/scope.cpp 				\
	$(liblumiprocmobjectsession_la_srcdir)/segment.cpp				\
	$(liblumiprocmobjectsession_la_srcdir)/segmentation.cpp				\
	$(liblumiprocmobjectsession_la_srcdir)/sess-manager-impl.cpp			\
	$(liblumiprocmobjectsession_la_srcdir)/session-impl.cpp				\
	$(liblumiprocmobjectsession_la_srcdir)/session-services.cpp 			\
	$(liblumiprocmobjectsession_la_srcdir)/session.cpp				\
	$(liblumiprocmobjectsession_la_srcdir)/simpleclip.cpp				\
	$(liblumiprocmobjectsession_la_srcdir)/track.cpp				\
	$(liblumiprocmobjectsession_la_srcdir)/wish.cpp




noinst_HEADERS +=									\
	$(liblumiproc_la_srcdir)/asset.hpp						\
	$(liblumiproc_la_srcdir)/asset/buildinstruct.hpp				\
	$(liblumiproc_la_srcdir)/asset/category.hpp					\
	$(liblumiproc_la_srcdir)/asset/clip.hpp						\
	$(liblumiproc_la_srcdir)/asset/codec.hpp					\
	$(liblumiproc_la_srcdir)/asset/compoundmedia.hpp				\
	$(liblumiproc_la_srcdir)/asset/dataset.hpp					\
	$(liblumiproc_la_srcdir)/asset/db.hpp						\
	$(liblumiproc_la_srcdir)/asset/effect.hpp					\
	$(liblumiproc_la_srcdir)/asset/media.hpp					\
	$(liblumiproc_la_srcdir)/asset/meta.hpp						\
	$(liblumiproc_la_srcdir)/asset/meta/time-grid.hpp				\
	$(liblumiproc_la_srcdir)/asset/pipe.hpp						\
	$(liblumiproc_la_srcdir)/asset/preview.hpp					\
	$(liblumiproc_la_srcdir)/asset/proc.hpp						\
	$(liblumiproc_la_srcdir)/asset/procpatt.hpp					\
	$(liblumiproc_la_srcdir)/asset/sequence.hpp					\
	$(liblumiproc_la_srcdir)/asset/struct.hpp					\
	$(liblumiproc_la_srcdir)/asset/timeline.hpp					\
	$(liblumiproc_la_srcdir)/asset/unknown.hpp					\
	$(liblumiproc_la_srcdir)/assetmanager.hpp					\
	$(liblumiproc_la_srcdir)/control/pathmanager.hpp				\
	$(liblumiproc_la_srcdir)/controllerfacade.hpp					\
	$(liblumiproc_la_srcdir)/engine/mask.hpp					\
	$(liblumiproc_la_srcdir)/engine/pluginadapter.hpp				\
	$(liblumiproc_la_srcdir)/engine/procnode.hpp					\
	$(liblumiproc_la_srcdir)/engine/projector.hpp					\
	$(liblumiproc_la_srcdir)/engine/renderengine.hpp				\
	$(liblumiproc_la_srcdir)/engine/rendergraph.hpp					\
	$(liblumiproc_la_srcdir)/engine/source.hpp					\
	$(liblumiproc_la_srcdir)/engine/trafo.hpp					\
	$(liblumiproc_la_srcdir)/facade.cpp						\
	$(liblumiproc_la_srcdir)/mobject/builder/assembler.hpp				\
	$(liblumiproc_la_srcdir)/mobject/builder/buildertool.hpp			\
	$(liblumiproc_la_srcdir)/mobject/builder/conmanager.hpp				\
	$(liblumiproc_la_srcdir)/mobject/builder/segmentation.hpp			\
	$(liblumiproc_la_srcdir)/mobject/builder/segmentation-tool.hpp			\
	$(liblumiproc_la_srcdir)/mobject/builder/toolfactory.hpp			\
	$(liblumiproc_la_srcdir)/mobject/builderfacade.hpp				\
	$(liblumiproc_la_srcdir)/mobject/explicitplacement.hpp				\
	$(liblumiproc_la_srcdir)/mobject/interpolator.hpp				\
	$(liblumiproc_la_srcdir)/mobject/mobject.hpp					\
	$(liblumiproc_la_srcdir)/mobject/parameter.hpp					\
	$(liblumiproc_la_srcdir)/mobject/paramprovider.hpp				\
	$(liblumiproc_la_srcdir)/mobject/placement.hpp					\
	$(liblumiproc_la_srcdir)/mobject/session.hpp					\
	$(liblumiproc_la_srcdir)/mobject/session/abstractmo.hpp				\
	$(liblumiproc_la_srcdir)/mobject/session/allocation.hpp				\
	$(liblumiproc_la_srcdir)/mobject/session/auto.hpp				\
	$(liblumiproc_la_srcdir)/mobject/session/clip.hpp				\
	$(liblumiproc_la_srcdir)/mobject/session/compoundclip.hpp			\
	$(liblumiproc_la_srcdir)/mobject/session/constraint.hpp				\
	$(liblumiproc_la_srcdir)/mobject/session/defs-manager.hpp			\
	$(liblumiproc_la_srcdir)/mobject/session/defs-registry.hpp			\
	$(liblumiproc_la_srcdir)/mobject/session/effect.hpp				\
	$(liblumiproc_la_srcdir)/mobject/session/fixedlocation.hpp			\
	$(liblumiproc_la_srcdir)/mobject/session/fixture.hpp				\
	$(liblumiproc_la_srcdir)/mobject/session/label.hpp				\
	$(liblumiproc_la_srcdir)/mobject/session/locatingpin.hpp			\
	$(liblumiproc_la_srcdir)/mobject/session/meta.hpp				\
	$(liblumiproc_la_srcdir)/mobject/session/mobjectfactory.hpp			\
	$(liblumiproc_la_srcdir)/mobject/session/placement-index-query-resolver.hpp	\
	$(liblumiproc_la_srcdir)/mobject/session/placement-index.hpp			\
	$(liblumiproc_la_srcdir)/mobject/session/plug.hpp				\
	$(liblumiproc_la_srcdir)/mobject/session/query-resolver.hpp			\
	$(liblumiproc_la_srcdir)/mobject/session/relativelocation.hpp			\
	$(liblumiproc_la_srcdir)/mobject/session/root.hpp				\
	$(liblumiproc_la_srcdir)/mobject/session/scope-path.hpp				\
	$(liblumiproc_la_srcdir)/mobject/session/scope.hpp				\
	$(liblumiproc_la_srcdir)/mobject/session/segment.hpp				\
	$(liblumiproc_la_srcdir)/mobject/session/segmentation.hpp			\
	$(liblumiproc_la_srcdir)/mobject/session/session-service-fetch.hpp		\
	$(liblumiproc_la_srcdir)/mobject/session/simpleclip.hpp				\
	$(liblumiproc_la_srcdir)/mobject/session/track.hpp				\
	$(liblumiproc_la_srcdir)/mobject/session/wish.hpp				\
	$(liblumiproc_la_srcdir)/play/dummy-image-generator.hpp				\
	$(liblumiproc_la_srcdir)/play/tick-service.hpp


liblumieraproc_la_srcdir = $(top_srcdir)/src/proc
lib_LTLIBRARIES += liblumieraproc.la

liblumieraproc_la_CFLAGS = $(AM_CFLAGS) $(LUMIERA_PROC_CFLAGS) -std=gnu99 -Wall -Wextra -Werror
liblumieraproc_la_CXXFLAGS = $(AM_CXXFLAGS) $(LUMIERA_PROC_CFLAGS) -Wall -Wextra

liblumieraproc_la_SOURCES =

liblumieraproc_la_LIBADD =			\
	$(LUMIERA_PROC_LIBS)			\
	liblumiproc.la				\
	liblumiprocasset.la			\
	liblumiproccontrol.la			\
	liblumiprocengine.la			\
	liblumiprocmobject.la			\
	liblumiprocmobjectbuilder.la		\
	liblumiprocmobjectsession.la		\
	liblumiprocplay.la
