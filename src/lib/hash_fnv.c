/*
 hash_fnv.c     - FNV hash functions

 Copyright (C)
   2010, 2011,                  Christian Thaeter <ct@pipapo.org>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, contact Christian Thaeter <ct@pipapo.org>.

The actual fnv functions are taken from Landon Curt Noll's original code,
slightly modified and adapted to C99; no copyright applies:
 ***
 *
 * Please do not copyright this code.  This code is in the public domain.
 *
 * LANDON CURT NOLL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO
 * EVENT SHALL LANDON CURT NOLL BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * By:
 *	chongo <Landon Curt Noll> /\oo/\
 *      http://www.isthe.com/chongo/
 *
 * Share and Enjoy!	:-)
*/


#include "hash_fnv.h"

#include <nobug.h>

uint64_t
hash_fnv64a_buf (const void *buf, size_t len, uint64_t hval)
{
  const unsigned char *bp = (const unsigned char *)buf;
  const unsigned char *be = bp + len;
  while (bp < be)
    {
      hval ^= (uint64_t)*bp++;
      hval *= HASH_FNV64_PRIME;
    }

  return hval;
}



uint32_t
hash_fnv32a_buf (const void *buf, size_t len, uint32_t hval)
{
  const unsigned char *bp = (const unsigned char *)buf;
  const unsigned char *be = bp + len;
  while (bp < be)
    {
      hval ^= (uint32_t)*bp++;
      hval *= HASH_FNV32_PRIME;
    }

  return hval;
}


uint64_t
hash_fnv64a_strn (const char* str, size_t len, uint64_t hval)
{
    const unsigned char *bp = (const unsigned char *)str;
    if (bp)
      while (*bp && len--)
        {
          hval ^= (uint64_t)*bp++;
          hval *= HASH_FNV64_PRIME;
        }

    return hval;
}



uint32_t
hash_fnv32a_strn (const char* str, size_t len, uint32_t hval)
{
    const unsigned char *bp = (const unsigned char *)str;
    if (bp)
      while (*bp && len--)
        {
          hval ^= (uint32_t)*bp++;
          hval *= HASH_FNV32_PRIME;
        }

    return hval;
}



uint64_t
hash_fnv64_xorfold (uint64_t hash, int bits)
{
  REQUIRE(bits <= 64);

  bits = 64-bits;

  uint64_t mask = ~0ULL>>bits;
  for (int todo = 32; bits && todo; todo >>= 1)
    if (bits >= todo)
      {
        hash = hash ^ hash>>todo;
        bits-=todo;
      }

  return hash & mask;
}


uint32_t
hash_fnv32_xorfold (uint32_t hash, int bits)
{
  REQUIRE (bits <= 32);

  bits = 32-bits;

  uint32_t mask = ~0ULL>>bits;
  for (int todo = 16; bits && todo; todo >>= 1)
    if (bits >= todo)
      {
        hash = hash ^ hash>>todo;
        bits-=todo;
      }

  return hash & mask;
}


uint64_t
hash_fnv64_retry (uint64_t hash, uint64_t limit)
{
  uint64_t retry_level= (UINT64_MAX / limit) * limit;

  while (hash >= retry_level)
      hash = (hash * HASH_FNV64_PRIME) + HASH_FNV64_BASE;

  return hash % limit;
}


uint32_t
hash_fnv32_retry (uint64_t hash, uint32_t limit)
{
  uint32_t retry_level= (UINT32_MAX / limit) * limit;

  while (hash >= retry_level)
      hash = (hash * HASH_FNV32_PRIME) + HASH_FNV32_BASE;

  return hash % limit;
}

/*
//      Local Variables:
//      mode: C
//      c-file-style: "gnu"
//      indent-tabs-mode: nil
//      End:
*/
