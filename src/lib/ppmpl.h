/*
  ppmpl.h  -  preprocessor meta programming library

  Copyright (C)         Lumiera.org
    2008,               Christian Thaeter <ct@pipapo.org>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef PPMPL_H
#define PPMPL_H

/**
 * @file
 * Preprocessor metaprogramming library. We define some useful preprocessor
 * tricks here.
 */


/**
 * Iterate over a list of macros.
 * @param p used to disambiguate up to three passes, use _, _P1_ or _P2_
 * @param ... list of macros to be expanded. The user has to supply a definition
 * in the form of PPMPL_FOREACH##p##macroname which shall expand to the desired text.
 *
 * This user defined macro shall be undefed after use.
 *
 * @example
 * #define PPMPL_FOREACH_P1_FOO(arg) arg,
 *
 * {PPMPL_FOREACH(P1, FOO(1), FOO(2), FOO(3)), -1}
 *
 * #undef PPMPL_FOREACH_P1_FOO
 *
 * Would expand to the sequence:
 * {1, 2, 3, -1}
 */
#define PPMPL_FOREACH(p, ...) PPMPL_FOREACH0(p, __VA_ARGS__, PPMPL_FOREACH_NIL))

/* internal */
#define PPMPL_FOREACH0(p, h, ...) PPMPL_FOREACH##p##h PPMPL_FOREACH1(p, __VA_ARGS__)
#define PPMPL_FOREACH1(p, h, ...) PPMPL_FOREACH##p##h PPMPL_FOREACH2(p, __VA_ARGS__)
#define PPMPL_FOREACH2(p, h, ...) PPMPL_FOREACH##p##h PPMPL_FOREACH3(p, __VA_ARGS__)
#define PPMPL_FOREACH3(p, h, ...) PPMPL_FOREACH##p##h PPMPL_FOREACH4(p, __VA_ARGS__)
#define PPMPL_FOREACH4(p, h, ...) PPMPL_FOREACH##p##h PPMPL_FOREACH5(p, __VA_ARGS__)
#define PPMPL_FOREACH5(p, h, ...) PPMPL_FOREACH##p##h PPMPL_FOREACH6(p, __VA_ARGS__)
#define PPMPL_FOREACH6(p, h, ...) PPMPL_FOREACH##p##h PPMPL_FOREACH7(p, __VA_ARGS__)
#define PPMPL_FOREACH7(p, h, ...) PPMPL_FOREACH##p##h PPMPL_FOREACH8(p, __VA_ARGS__)
#define PPMPL_FOREACH8(p, h, ...) PPMPL_FOREACH##p##h PPMPL_FOREACH9(p, __VA_ARGS__)
#define PPMPL_FOREACH9(p, h, ...) PPMPL_FOREACH##p##h PPMPL_FOREACH10(p, __VA_ARGS__)
#define PPMPL_FOREACH10(p, h, ...) PPMPL_FOREACH##p##h PPMPL_FOREACH11(p, __VA_ARGS__)
#define PPMPL_FOREACH11(p, h, ...) PPMPL_FOREACH##p##h PPMPL_FOREACH12(p, __VA_ARGS__)
#define PPMPL_FOREACH12(p, h, ...) PPMPL_FOREACH##p##h PPMPL_FOREACH13(p, __VA_ARGS__)
#define PPMPL_FOREACH13(p, h, ...) PPMPL_FOREACH##p##h PPMPL_FOREACH14(p, __VA_ARGS__)
#define PPMPL_FOREACH14(p, h, ...) PPMPL_FOREACH##p##h PPMPL_FOREACH15(p, __VA_ARGS__)
#define PPMPL_FOREACH15(p, h, ...) PPMPL_FOREACH##p##h
#define PPMPL_FOREACH_
#define PPMPL_FOREACH_P1_
#define PPMPL_FOREACH_P2_
#define PPMPL_FOREACH_PPMPL_FOREACH_NIL PPMPL_FOREACH_FINAL(
#define PPMPL_FOREACH_P1_PPMPL_FOREACH_NIL PPMPL_FOREACH_FINAL(
#define PPMPL_FOREACH_P2_PPMPL_FOREACH_NIL PPMPL_FOREACH_FINAL(
#define PPMPL_FOREACH_FINAL(...)

#endif
/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
