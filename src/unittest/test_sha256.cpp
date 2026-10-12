// BlockyOS engine
// SPDX-License-Identifier: MIT
// Copyright (c) 2026 BlockyOS contributors

// Tests the bundled SHA-224/SHA-256 in lib/sha256 directly. SRP login
// (src/util/srp.cpp) always uses it, and hashing::sha256 falls back to it
// when the engine is built without OpenSSL, so a wrong digest breaks
// authentication and builtin-file integrity checks. hashing::sha256 is not
// used here because it may route to OpenSSL instead.

#include "test.h"

#include "my_sha256.h"
#include "util/hex.h"

#include <string>

class TestSha256 : public TestBase
{
public:
	TestSha256() { TestManager::registerTestModule(this); }
	const char *getName() { return "TestSha256"; }

	void runTests(IGameDef *gamedef);

	void testKnownVectors();
	void testSplitUpdates();
	void testMillionAInChunks();
	void testPaddingBoundaries();
	void testFinalRejectsReuse();
};

static TestSha256 g_test_instance;

void TestSha256::runTests(IGameDef *gamedef)
{
	TEST(testKnownVectors);
	TEST(testSplitUpdates);
	TEST(testMillionAInChunks);
	TEST(testPaddingBoundaries);
	TEST(testFinalRejectsReuse);
}

////////////////////////////////////////////////////////////////////////////////

namespace {

struct Vector {
	std::string message;
	const char *sha224;
	const char *sha256;
};

// The 448-bit two-block message from the FIPS 180-2/180-4 examples.
const char *const MSG_448 =
	"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";

std::string digest_hex(int bits, const std::string &msg)
{
	unsigned char md[SHA256_DIGEST_LENGTH];
	const auto *in = reinterpret_cast<const unsigned char *>(msg.data());
	if (bits == 224) {
		UASSERT(SHA224(in, msg.size(), md) == md);
		return hex_encode(reinterpret_cast<char *>(md), SHA224_DIGEST_LENGTH);
	}
	UASSERT(SHA256(in, msg.size(), md) == md);
	return hex_encode(reinterpret_cast<char *>(md), SHA256_DIGEST_LENGTH);
}

// Feeds msg through Init/Update/Final, with the first update `split` bytes long.
std::string split_digest_hex(int bits, const std::string &msg, size_t split)
{
	SHA256_CTX c;
	unsigned char md[SHA256_DIGEST_LENGTH];
	UASSERT((bits == 224 ? SHA224_Init(&c) : SHA256_Init(&c)) == 1);
	UASSERT(SHA256_Update(&c, msg.data(), split) == 1);
	UASSERT(SHA256_Update(&c, msg.data() + split, msg.size() - split) == 1);
	UASSERT((bits == 224 ? SHA224_Final(md, &c) : SHA256_Final(md, &c)) == 1);
	size_t len = bits == 224 ? SHA224_DIGEST_LENGTH : SHA256_DIGEST_LENGTH;
	return hex_encode(reinterpret_cast<char *>(md), len);
}

} // namespace

// Published FIPS 180-4 / NIST CSRC example digests.
void TestSha256::testKnownVectors()
{
	const Vector vectors[] = {
		{"",
			"d14a028c2a3a2bc9476102bb288234c415a2b01f828ea62ac5b3e42f",
			"e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"},
		{"abc",
			"23097d223405d8228642a477bda255b32aadbce4bda0b3f7e36c9da7",
			"ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad"},
		{MSG_448,
			"75388b16512776cc5dba5da1fd890150b0c6455cb4f58b1952522525",
			"248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1"},
	};
	for (const auto &v : vectors) {
		UASSERTEQ(std::string, digest_hex(224, v.message), v.sha224);
		UASSERTEQ(std::string, digest_hex(256, v.message), v.sha256);
	}
}

// Every split point of a two-block message must give the same digest, so
// partial-block buffering in Update cannot drop or reorder bytes.
void TestSha256::testSplitUpdates()
{
	const std::string msg = MSG_448;
	for (size_t split = 0; split <= msg.size(); split++) {
		UASSERTEQ(std::string, split_digest_hex(224, msg, split),
			"75388b16512776cc5dba5da1fd890150b0c6455cb4f58b1952522525");
		UASSERTEQ(std::string, split_digest_hex(256, msg, split),
			"248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1");
	}
}

// One million 'a' (FIPS 180-4 / NIST example), fed in uneven chunks that
// start and end at many offsets within the 64-byte block.
void TestSha256::testMillionAInChunks()
{
	const std::string chunk(1000, 'a');
	const size_t sizes[] = {1, 3, 7, 55, 56, 63, 64, 65, 127, 128, 129, 997};
	const size_t total = 1000000;

	for (int bits : {224, 256}) {
		SHA256_CTX c;
		unsigned char md[SHA256_DIGEST_LENGTH];
		UASSERT((bits == 224 ? SHA224_Init(&c) : SHA256_Init(&c)) == 1);
		size_t fed = 0;
		for (size_t i = 0; fed < total; i++) {
			size_t n = sizes[i % (sizeof(sizes) / sizeof(sizes[0]))];
			if (n > total - fed)
				n = total - fed;
			UASSERT(SHA256_Update(&c, chunk.data(), n) == 1);
			fed += n;
		}
		UASSERT((bits == 224 ? SHA224_Final(md, &c) : SHA256_Final(md, &c)) == 1);

		if (bits == 224) {
			UASSERTEQ(std::string,
				hex_encode(reinterpret_cast<char *>(md), SHA224_DIGEST_LENGTH),
				"20794655980c91d8bbb4c1ea97618a4bf03f42581948b2ee4ee7ad67");
		} else {
			UASSERTEQ(std::string,
				hex_encode(reinterpret_cast<char *>(md), SHA256_DIGEST_LENGTH),
				"cdc76e5c9914fb9281a1c7e284d73e67f1809a48a497200e046d39ccc7112cd0");
		}
	}
}

// Messages of n 'a' bytes around the padding boundaries: 55 bytes is the
// longest that pads within one block, 56 forces a second block, 64 is a full
// block. Digests cross-checked with two independent implementations
// (shasum and Python hashlib).
void TestSha256::testPaddingBoundaries()
{
	const struct {
		size_t n;
		const char *sha224;
		const char *sha256;
	} cases[] = {
		{55, "fb0bd626a70c28541dfa781bb5cc4d7d7f56622a58f01a0b1ddd646f",
			"9f4390f8d30c2dd92ec9f095b65e2b9ae9b0a925a5258e241c9f1e910f734318"},
		{56, "d40854fc9caf172067136f2e29e1380b14626bf6f0dd06779f820dcd",
			"b35439a4ac6f0948b6d6f9e3c6af0f5f590ce20f1bde7090ef7970686ec6738a"},
		{57, "b5d09534784ab6578128bce7f28a96a56e3b45c4f734f74739076249",
			"f13b2d724659eb3bf47f2dd6af1accc87b81f09f59f2b75e5c0bed6589dfe8c6"},
		{63, "1d4e051f4d6fed2a63fd2421e65834cec00d64456553de3496ae8b1d",
			"7d3e74a05d7db15bce4ad9ec0658ea98e3f06eeecf16b4c6fff2da457ddc2f34"},
		{64, "a88cd5cde6d6fe9136a4e58b49167461ea95d388ca2bdb7afdc3cbf4",
			"ffe054fe7ae0cb6dc65c3af9b61d5209f439851db43d0ba5997337df154668eb"},
		{65, "ff8716f600af42959d0efb52e1f21b01bb328733009344d511c299fb",
			"635361c48bb9eab14198e76ea8ab7f1a41685d6ad62aa9146d301d4f17eb0ae0"},
		{119, "e000e6709d26667b631faa7fc1bd404eb4774003c5fb4f51a0184875",
			"31eba51c313a5c08226adf18d4a359cfdfd8d2e816b13f4af952f7ea6584dcfb"},
		{120, "66924e30a9929327e7a6cf03747397226ed2efc180ebe3dea7132a79",
			"2f3d335432c70b580af0e8e1b3674a7c020d683aa5f73aaaedfdc55af904c21c"},
	};
	for (const auto &tc : cases) {
		const std::string msg(tc.n, 'a');
		UASSERTEQ(std::string, digest_hex(224, msg), tc.sha224);
		UASSERTEQ(std::string, digest_hex(256, msg), tc.sha256);
		// Same message with the boundary-crossing byte in its own update.
		UASSERTEQ(std::string, split_digest_hex(256, msg, tc.n - 1), tc.sha256);
	}
}

// Final wipes the context; finishing it again must fail rather than emit a
// digest of nothing.
void TestSha256::testFinalRejectsReuse()
{
	SHA256_CTX c;
	unsigned char md[SHA256_DIGEST_LENGTH];
	UASSERT(SHA256_Init(&c) == 1);
	UASSERT(SHA256_Final(md, &c) == 1);
	UASSERT(SHA256_Final(md, &c) == 0);
}
