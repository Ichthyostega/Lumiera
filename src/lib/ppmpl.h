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
 *
 * One can not recursively nest preprocessor macros. To allow this we define PPMPL_FOREACH_L1
 * to PPMPL_FOREACH_L2 with the same semantics as PPMPL_FOREACH, This allowes to nest the
 * FOREACH loop up to three nesting levels.
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




#define PPMPL_FOREACH_L1(p, ...) PPMPL_FOREACH_L1_0(p, __VA_ARGS__, PPMPL_FOREACH_L1_NIL))

/* internal */
#define PPMPL_FOREACH_L1_0(p, h, ...) PPMPL_FOREACH_L1##p##h PPMPL_FOREACH_L1_1(p, __VA_ARGS__)
#define PPMPL_FOREACH_L1_1(p, h, ...) PPMPL_FOREACH_L1##p##h PPMPL_FOREACH_L1_2(p, __VA_ARGS__)
#define PPMPL_FOREACH_L1_2(p, h, ...) PPMPL_FOREACH_L1##p##h PPMPL_FOREACH_L1_3(p, __VA_ARGS__)
#define PPMPL_FOREACH_L1_3(p, h, ...) PPMPL_FOREACH_L1##p##h PPMPL_FOREACH_L1_4(p, __VA_ARGS__)
#define PPMPL_FOREACH_L1_4(p, h, ...) PPMPL_FOREACH_L1##p##h PPMPL_FOREACH_L1_5(p, __VA_ARGS__)
#define PPMPL_FOREACH_L1_5(p, h, ...) PPMPL_FOREACH_L1##p##h PPMPL_FOREACH_L1_6(p, __VA_ARGS__)
#define PPMPL_FOREACH_L1_6(p, h, ...) PPMPL_FOREACH_L1##p##h PPMPL_FOREACH_L1_7(p, __VA_ARGS__)
#define PPMPL_FOREACH_L1_7(p, h, ...) PPMPL_FOREACH_L1##p##h PPMPL_FOREACH_L1_8(p, __VA_ARGS__)
#define PPMPL_FOREACH_L1_8(p, h, ...) PPMPL_FOREACH_L1##p##h PPMPL_FOREACH_L1_9(p, __VA_ARGS__)
#define PPMPL_FOREACH_L1_9(p, h, ...) PPMPL_FOREACH_L1##p##h PPMPL_FOREACH_L1_10(p, __VA_ARGS__)
#define PPMPL_FOREACH_L1_10(p, h, ...) PPMPL_FOREACH_L1##p##h PPMPL_FOREACH_L1_11(p, __VA_ARGS__)
#define PPMPL_FOREACH_L1_11(p, h, ...) PPMPL_FOREACH_L1##p##h PPMPL_FOREACH_L1_12(p, __VA_ARGS__)
#define PPMPL_FOREACH_L1_12(p, h, ...) PPMPL_FOREACH_L1##p##h PPMPL_FOREACH_L1_13(p, __VA_ARGS__)
#define PPMPL_FOREACH_L1_13(p, h, ...) PPMPL_FOREACH_L1##p##h PPMPL_FOREACH_L1_14(p, __VA_ARGS__)
#define PPMPL_FOREACH_L1_14(p, h, ...) PPMPL_FOREACH_L1##p##h PPMPL_FOREACH_L1_15(p, __VA_ARGS__)
#define PPMPL_FOREACH_L1_15(p, h, ...) PPMPL_FOREACH_L1##p##h
#define PPMPL_FOREACH_L1_
#define PPMPL_FOREACH_L1_P1_
#define PPMPL_FOREACH_L1_P2_
#define PPMPL_FOREACH_L1_PPMPL_FOREACH_L1_NIL PPMPL_FOREACH_L1_FINAL(
#define PPMPL_FOREACH_L1_P1_PPMPL_FOREACH_L1_NIL PPMPL_FOREACH_L1_FINAL(
#define PPMPL_FOREACH_L1_P2_PPMPL_FOREACH_L1_NIL PPMPL_FOREACH_L1_FINAL(
#define PPMPL_FOREACH_L1_FINAL(...)




#define PPMPL_FOREACH_L2(p, ...) PPMPL_FOREACH_L2_0(p, __VA_ARGS__, PPMPL_FOREACH_L2_NIL))

/* internal */
#define PPMPL_FOREACH_L2_0(p, h, ...) PPMPL_FOREACH_L2##p##h PPMPL_FOREACH_L2_1(p, __VA_ARGS__)
#define PPMPL_FOREACH_L2_1(p, h, ...) PPMPL_FOREACH_L2##p##h PPMPL_FOREACH_L2_2(p, __VA_ARGS__)
#define PPMPL_FOREACH_L2_2(p, h, ...) PPMPL_FOREACH_L2##p##h PPMPL_FOREACH_L2_3(p, __VA_ARGS__)
#define PPMPL_FOREACH_L2_3(p, h, ...) PPMPL_FOREACH_L2##p##h PPMPL_FOREACH_L2_4(p, __VA_ARGS__)
#define PPMPL_FOREACH_L2_4(p, h, ...) PPMPL_FOREACH_L2##p##h PPMPL_FOREACH_L2_5(p, __VA_ARGS__)
#define PPMPL_FOREACH_L2_5(p, h, ...) PPMPL_FOREACH_L2##p##h PPMPL_FOREACH_L2_6(p, __VA_ARGS__)
#define PPMPL_FOREACH_L2_6(p, h, ...) PPMPL_FOREACH_L2##p##h PPMPL_FOREACH_L2_7(p, __VA_ARGS__)
#define PPMPL_FOREACH_L2_7(p, h, ...) PPMPL_FOREACH_L2##p##h PPMPL_FOREACH_L2_8(p, __VA_ARGS__)
#define PPMPL_FOREACH_L2_8(p, h, ...) PPMPL_FOREACH_L2##p##h PPMPL_FOREACH_L2_9(p, __VA_ARGS__)
#define PPMPL_FOREACH_L2_9(p, h, ...) PPMPL_FOREACH_L2##p##h PPMPL_FOREACH_L2_10(p, __VA_ARGS__)
#define PPMPL_FOREACH_L2_10(p, h, ...) PPMPL_FOREACH_L2##p##h PPMPL_FOREACH_L2_11(p, __VA_ARGS__)
#define PPMPL_FOREACH_L2_11(p, h, ...) PPMPL_FOREACH_L2##p##h PPMPL_FOREACH_L2_12(p, __VA_ARGS__)
#define PPMPL_FOREACH_L2_12(p, h, ...) PPMPL_FOREACH_L2##p##h PPMPL_FOREACH_L2_13(p, __VA_ARGS__)
#define PPMPL_FOREACH_L2_13(p, h, ...) PPMPL_FOREACH_L2##p##h PPMPL_FOREACH_L2_14(p, __VA_ARGS__)
#define PPMPL_FOREACH_L2_14(p, h, ...) PPMPL_FOREACH_L2##p##h PPMPL_FOREACH_L2_15(p, __VA_ARGS__)
#define PPMPL_FOREACH_L2_15(p, h, ...) PPMPL_FOREACH_L2##p##h
#define PPMPL_FOREACH_L2_
#define PPMPL_FOREACH_L2_P1_
#define PPMPL_FOREACH_L2_P2_
#define PPMPL_FOREACH_L2_PPMPL_FOREACH_L2_NIL PPMPL_FOREACH_L2_FINAL(
#define PPMPL_FOREACH_L2_P1_PPMPL_FOREACH_L2_NIL PPMPL_FOREACH_L2_FINAL(
#define PPMPL_FOREACH_L2_P2_PPMPL_FOREACH_L2_NIL PPMPL_FOREACH_L2_FINAL(
#define PPMPL_FOREACH_L2_FINAL(...)

/**
 * Canonical preprocessor concat implementation which evaluates its arguments
 */
#define PPMPL_CAT(a,b) PPMPL_CAT_(a,b)
#define PPMPL_CAT_(a,b) a##b

#endif
/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
