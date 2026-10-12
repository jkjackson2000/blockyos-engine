# Modified for BlockyOS by BlockyOS contributors, 2026-10-12: build the original
# MIT-licensed SHA-224/SHA-256 implementation; the endian.h check is no longer needed.
project(sha256 C)

add_library(sha256 STATIC sha256.c my_sha256.h)

target_include_directories(sha256 INTERFACE .)
