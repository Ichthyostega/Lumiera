/*
  HASH-FNV.h  -  FNV hash functions

  original by           chongo <Landon Curt Noll> /\oo/\
                          http://www.isthe.com/chongo/
  adapted for Lumiera
    2010, 2011          Christian Thaeter <ct@pipapo.org>

  Please do not copyright this code.  This code is in the public domain.

  LANDON CURT NOLL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
  INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO
  EVENT SHALL LANDON CURT NOLL BE LIABLE FOR ANY SPECIAL, INDIRECT OR
  CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
  USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
  OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
  PERFORMANCE OF THIS SOFTWARE.

  Share and Enjoy! :-)

*/


/** @file hash-fnv.h
 ** Fowler-Noll-Vo Hashes.
 ** FNV is a non-cryptographic hash function created by Glenn Fowler, Landon Curt Noll, and Phong Vo.
 */


#ifndef HASH_FNV_H
#define HASH_FNV_H

#include <stdlib.h>  /* for size_t */
#include <inttypes.h>

#define HASH_FNV64_BASE ((uint64_t)14695981039346656037ULL)
#define HASH_FNV32_BASE ((uint32_t)2166136261UL)

#define HASH_FNV64_PRIME ((uint64_t)1099511628211ULL)
#define HASH_FNV32_PRIME ((uint32_t)16777619UL)


/**
 * FNV-1a 64 bit hash over a buffer.
 * @param buf start of the buffer
 * @param len size of the buffer
 * @param hval previous hash value when incremental hashing or HASH_FNV64_BASE when starting a new hash
 * @return new hash value
 */
uint64_t
hash_fnv64a_buf (const void *buf, size_t len, uint64_t hval);

/**
 * FNV-1a 64 bit hash over a zero terminated string.
 * @param str start of the buffer
 * @param len maximum size to be processed
 * @param hval previous hash value when incremental hashing or HASH_FNV64_BASE when starting a new hash
 * @return new hash value
 */
uint64_t
hash_fnv64a_strn (const char* str, size_t len, uint64_t hval);


/**
 * FNV-1a 32 bit hash over a buffer.
 * @param buf start of the buffer
 * @param len size of the buffer
 * @param hval previous hash value when incremental hashing or HASH_FNV32_BASE when starting a new hash
 * @return new hash value
 */
uint32_t
hash_fnv32a_buf (const void *buf, size_t len, uint32_t hval);


/**
 * FNV-1a 32 bit hash over a zero terminated string.
 * @param str start of the buffer
 * @param len maximum size to be processed
 * @param hval previous hash value when incremental hashing or HASH_FNV32_BASE when starting a new hash
 * @return new hash value
 */
uint32_t
hash_fnv32a_strn (const char* str, size_t len, uint32_t hval);



/**
 * reduce a hash value to n bits.
 * Uses the xor folding method to stash a hash value together, this preserves unbiased hash distribution
 * @param hash result from one of the 64 bit hash functions above
 * @param bits number of significat bits for the result
 * @result hashvalue with no more than 'bits' significant bits
 */
uint64_t
hash_fnv64_xorfold (uint64_t hash, int bits);


/**
 * reduce a hash value to n bits.
 * Uses the xor folding method to stash a hash value together, this preserves unbiased hash distribution
 * @param hash result from one of the 32 bit hash functions above
 * @param bits number of significat bits for the result
 * @result hashvalue with no more than 'bits' significant bits
 */
uint32_t
hash_fnv32_xorfold (uint32_t hash, int bits);


/**
 * reduce hash to be within 0 to limit-1.
 * Uses the retry method to limit a hash value, this preserves unbiased hash distribution.
 * @param hash result from one of the 64 bit hash functions above
 * @param limit upper limit plus one for the result
 * @result hashvalue in the range from 0 to limit-1
 */
uint64_t
hash_fnv64_retry (uint64_t hash, uint64_t limit);


/**
 * reduce hash to be within 0 to limit-1.
 * Uses the retry method to limit a hash value, this preserves unbiased hash distribution.
 * @param hash result from one of the 32 bit hash functions above
 * @param limit upper limit plus one for the result
 * @result hashvalue in the range from 0 to limit-1
 */
uint32_t
hash_fnv32_retry (uint64_t hash, uint32_t limit);



/*
PLANNED? 128 bit and 256bit hashes
128 bit FNV_prime = 288 + 28 + 0x3b = 309485009821345068724781371

256 bit FNV_prime = 2168 + 28 + 0x63 = 374144419156711147060143317175368453031918731002211

128 bit offset_basis = 144066263297769815596495629667062367629

256 bit offset_basis =
100029257958052580907070968620625704837092796014241193945225284501741471925557

*/



#endif

/*
//      Local Variables:
//      mode: C
//      c-file-style: "gnu"
//      indent-tabs-mode: nil
//      End:
*/
