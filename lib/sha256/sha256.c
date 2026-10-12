/*
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2026 BlockyOS contributors
 *
 * SHA-224 and SHA-256 as specified in NIST FIPS 180-4, written from the
 * specification for the BlockyOS engine. Section numbers below refer to
 * FIPS 180-4 (August 2015). License text: see my_sha256.h.
 */

#include "my_sha256.h"

#include <string.h>

/* Section 4.2.2: the first 32 bits of the fractional parts of the cube roots
 * of the first 64 prime numbers. */
static const uint32_t round_constants[64] = {
	0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
	0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
	0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
	0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
	0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
	0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
	0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
	0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
	0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
	0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
	0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
	0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
	0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
	0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
	0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
	0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2,
};

/* Section 5.3.2: initial hash value for SHA-224. */
static const uint32_t sha224_initial[8] = {
	0xc1059ed8, 0x367cd507, 0x3070dd17, 0xf70e5939,
	0xffc00b31, 0x68581511, 0x64f98fa7, 0xbefa4fa4,
};

/* Section 5.3.3: initial hash value for SHA-256. */
static const uint32_t sha256_initial[8] = {
	0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
	0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19,
};

/* Section 3.2: rotate right, 0 < n < 32. */
static uint32_t rotr(uint32_t x, unsigned int n)
{
	return (x >> n) | (x << (32 - n));
}

/* Section 4.1.2: the six logical functions. */
static uint32_t ch(uint32_t x, uint32_t y, uint32_t z)
{
	return (x & y) ^ (~x & z);
}

static uint32_t maj(uint32_t x, uint32_t y, uint32_t z)
{
	return (x & y) ^ (x & z) ^ (y & z);
}

static uint32_t big_sigma0(uint32_t x)
{
	return rotr(x, 2) ^ rotr(x, 13) ^ rotr(x, 22);
}

static uint32_t big_sigma1(uint32_t x)
{
	return rotr(x, 6) ^ rotr(x, 11) ^ rotr(x, 25);
}

static uint32_t small_sigma0(uint32_t x)
{
	return rotr(x, 7) ^ rotr(x, 18) ^ (x >> 3);
}

static uint32_t small_sigma1(uint32_t x)
{
	return rotr(x, 17) ^ rotr(x, 19) ^ (x >> 10);
}

/* SHA-2 words are big-endian (section 3.1); convert byte by byte so the code
 * does not depend on host byte order or alignment. */
static uint32_t read_be32(const unsigned char *p)
{
	return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) |
		((uint32_t)p[2] << 8) | (uint32_t)p[3];
}

static void write_be32(unsigned char *p, uint32_t v)
{
	p[0] = (unsigned char)(v >> 24);
	p[1] = (unsigned char)(v >> 16);
	p[2] = (unsigned char)(v >> 8);
	p[3] = (unsigned char)v;
}

/* Section 6.2.2: process one 512-bit message block. */
static void compress(uint32_t state[8], const unsigned char *block)
{
	uint32_t w[64];
	uint32_t a, b, c, d, e, f, g, h;
	int t;

	/* Step 1: prepare the message schedule. */
	for (t = 0; t < 16; t++)
		w[t] = read_be32(block + 4 * t);
	for (t = 16; t < 64; t++)
		w[t] = small_sigma1(w[t - 2]) + w[t - 7] +
			small_sigma0(w[t - 15]) + w[t - 16];

	/* Step 2: initialize the working variables. */
	a = state[0];
	b = state[1];
	c = state[2];
	d = state[3];
	e = state[4];
	f = state[5];
	g = state[6];
	h = state[7];

	/* Step 3: 64 rounds. */
	for (t = 0; t < 64; t++) {
		uint32_t t1 = h + big_sigma1(e) + ch(e, f, g) + round_constants[t] + w[t];
		uint32_t t2 = big_sigma0(a) + maj(a, b, c);
		h = g;
		g = f;
		f = e;
		e = d + t1;
		d = c;
		c = b;
		b = a;
		a = t1 + t2;
	}

	/* Step 4: compute the next intermediate hash value. */
	state[0] += a;
	state[1] += b;
	state[2] += c;
	state[3] += d;
	state[4] += e;
	state[5] += f;
	state[6] += g;
	state[7] += h;
}

/* Overwrite the context so hashed secrets do not linger. The volatile
 * pointer keeps the compiler from dropping the stores. */
static void wipe(SHA256_CTX *c)
{
	volatile unsigned char *p = (volatile unsigned char *)c;
	size_t i;

	for (i = 0; i < sizeof(*c); i++)
		p[i] = 0;
}

static int start(SHA256_CTX *c, const uint32_t initial[8], unsigned int digest_len)
{
	if (c == NULL)
		return 0;
	memset(c, 0, sizeof(*c));
	memcpy(c->state, initial, sizeof(c->state));
	c->digest_len = digest_len;
	return 1;
}

int SHA224_Init(SHA256_CTX *c)
{
	return start(c, sha224_initial, SHA224_DIGEST_LENGTH);
}

int SHA256_Init(SHA256_CTX *c)
{
	return start(c, sha256_initial, SHA256_DIGEST_LENGTH);
}

int SHA256_Update(SHA256_CTX *c, const void *data, size_t len)
{
	const unsigned char *in = (const unsigned char *)data;

	if (c == NULL || c->block_used >= sizeof(c->block))
		return 0;
	if (len == 0)
		return 1;
	if (in == NULL)
		return 0;

	/* FIPS 180-4 limits messages to fewer than 2^64 bits, so the count is
	 * kept modulo 2^64. */
	c->length_bits += (uint64_t)len << 3;

	/* Top up a partially filled block first. */
	if (c->block_used > 0) {
		size_t take = sizeof(c->block) - c->block_used;
		if (take > len)
			take = len;
		memcpy(c->block + c->block_used, in, take);
		c->block_used += take;
		in += take;
		len -= take;
		if (c->block_used < sizeof(c->block))
			return 1;
		compress(c->state, c->block);
		c->block_used = 0;
	}

	/* Hash whole blocks straight from the input. */
	while (len >= sizeof(c->block)) {
		compress(c->state, in);
		in += sizeof(c->block);
		len -= sizeof(c->block);
	}

	/* Keep the tail for the next call. */
	if (len > 0) {
		memcpy(c->block, in, len);
		c->block_used = len;
	}
	return 1;
}

int SHA224_Update(SHA256_CTX *c, const void *data, size_t len)
{
	return SHA256_Update(c, data, len);
}

int SHA256_Final(unsigned char *md, SHA256_CTX *c)
{
	uint64_t length_bits;
	unsigned int i;

	if (c == NULL)
		return 0;
	if (md == NULL || c->block_used >= sizeof(c->block) ||
			(c->digest_len != SHA224_DIGEST_LENGTH &&
			 c->digest_len != SHA256_DIGEST_LENGTH)) {
		wipe(c);
		return 0;
	}

	/* Section 5.1.1: append a single 1 bit, then zero bits until the length
	 * is 448 mod 512, then the 64-bit big-endian message length. */
	length_bits = c->length_bits;
	c->block[c->block_used++] = 0x80;
	if (c->block_used > 56) {
		memset(c->block + c->block_used, 0, sizeof(c->block) - c->block_used);
		compress(c->state, c->block);
		c->block_used = 0;
	}
	memset(c->block + c->block_used, 0, 56 - c->block_used);
	write_be32(c->block + 56, (uint32_t)(length_bits >> 32));
	write_be32(c->block + 60, (uint32_t)length_bits);
	compress(c->state, c->block);

	/* Sections 6.2.2 and 6.3: SHA-256 outputs H0..H7, SHA-224 H0..H6. */
	for (i = 0; i < c->digest_len / 4; i++)
		write_be32(md + 4 * i, c->state[i]);

	wipe(c);
	return 1;
}

int SHA224_Final(unsigned char *md, SHA256_CTX *c)
{
	return SHA256_Final(md, c);
}

static unsigned char *one_shot(int (*init)(SHA256_CTX *),
		const unsigned char *d, size_t n, unsigned char *md)
{
	SHA256_CTX c;

	if (md == NULL)
		return NULL;
	if (!init(&c) || !SHA256_Update(&c, d, n) || !SHA256_Final(md, &c)) {
		wipe(&c);
		return NULL;
	}
	return md;
}

unsigned char *SHA224(const unsigned char *d, size_t n, unsigned char *md)
{
	return one_shot(SHA224_Init, d, n, md);
}

unsigned char *SHA256(const unsigned char *d, size_t n, unsigned char *md)
{
	return one_shot(SHA256_Init, d, n, md);
}
