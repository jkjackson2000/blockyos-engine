/*
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2026 BlockyOS contributors
 *
 * SHA-224 and SHA-256 as specified in NIST FIPS 180-4.
 *
 * Original implementation written for the BlockyOS engine (a fork of
 * Luanti); it replaces the upstream lib/sha256 code. See BLOCKYOS_NOTICES.md.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef BLOCKYOS_MY_SHA256_H
#define BLOCKYOS_MY_SHA256_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SHA224_DIGEST_LENGTH 28
#define SHA256_DIGEST_LENGTH 32

/*
 * Callers use the familiar SHA256_Init()-style names. Those names are also
 * exported by OpenSSL's libcrypto, which the engine links when ENABLE_OPENSSL
 * is on. Mapping them to distinct link symbols keeps the two implementations
 * from interposing on each other, so code that includes this header always
 * gets this implementation.
 */
#define SHA224_Init   blockyos_sha224_init
#define SHA224_Update blockyos_sha224_update
#define SHA224_Final  blockyos_sha224_final
#define SHA224        blockyos_sha224
#define SHA256_Init   blockyos_sha256_init
#define SHA256_Update blockyos_sha256_update
#define SHA256_Final  blockyos_sha256_final
#define SHA256        blockyos_sha256

/* Shared by SHA-224 and SHA-256; treat the fields as private. */
typedef struct blockyos_sha256_ctx {
	uint32_t state[8];         /* intermediate hash value H(i) */
	uint64_t length_bits;      /* message length so far, in bits */
	unsigned char block[64];   /* pending input that is not yet a full block */
	size_t block_used;         /* number of bytes pending in block */
	unsigned int digest_len;   /* 28 for SHA-224, 32 for SHA-256 */
} SHA256_CTX;

/* The Init/Update/Final functions return 1 on success and 0 on misuse. */
int SHA224_Init(SHA256_CTX *c);
int SHA224_Update(SHA256_CTX *c, const void *data, size_t len);
int SHA224_Final(unsigned char *md, SHA256_CTX *c);

int SHA256_Init(SHA256_CTX *c);
int SHA256_Update(SHA256_CTX *c, const void *data, size_t len);
int SHA256_Final(unsigned char *md, SHA256_CTX *c);

/*
 * One-shot digests of d[0..n). md must have room for the digest; the
 * functions return md, or NULL if md is NULL.
 */
unsigned char *SHA224(const unsigned char *d, size_t n, unsigned char *md);
unsigned char *SHA256(const unsigned char *d, size_t n, unsigned char *md);

#ifdef __cplusplus
}
#endif

#endif /* BLOCKYOS_MY_SHA256_H */
