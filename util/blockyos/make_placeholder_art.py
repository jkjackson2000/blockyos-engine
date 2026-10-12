#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
# Copyright (c) 2026 BlockyOS contributors
"""Generate the BlockyOS placeholder logo, header, and icons.

These are placeholders until the production BlockyOS logo assets land
(jkjackson2000/blockyos#11). Everything is drawn here from hand-placed pixel
grids: no fonts, images, or other third-party inputs. The output replaces the
upstream Luanti art at the same paths so engine code that loads it needs no
change.

Usage (from anywhere): python3 util/blockyos/make_placeholder_art.py

Python 3 standard library only. PNGs are written as minimal palette images
(no ancillary chunks, smallest of several filter/zlib choices) so that
util/ci/check_png_optimized.sh finds nothing to gain.
"""

import os
import struct
import zlib

REPO = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", ".."))

# Palette: the BlockyOS prototype brand colors (grass top, dirt side, deep
# green shadow, cream wordmark). None is fully transparent.
COLORS = {
	".": None,
	"h": (0x93, 0xc2, 0x63),  # grass highlight
	"G": (0x78, 0xa6, 0x4e),  # grass
	"D": (0xad, 0x84, 0x52),  # dirt
	"d": (0x8c, 0x66, 0x3e),  # dirt, bottom edge
	"S": (0x39, 0x52, 0x34),  # block shadow
	"W": (0xff, 0xf8, 0xe4),  # wordmark cream
	"w": (0x34, 0x43, 0x2d),  # wordmark shadow
	"t": (0xe0, 0xd8, 0xbc),  # placeholder tag
}

# The block, after the prototype's brand mark: a dirt square with a straight
# grass band across the top third and a drop shadow down and to the right.
# Deliberately plain (no texture) while it is a placeholder. 16x16 so it
# scales by whole pixels to 64/128/256.
BLOCK = [
	"hhhhhhhhhhhhhh..",
	"GGGGGGGGGGGGGG..",
	"GGGGGGGGGGGGGGSS",
	"GGGGGGGGGGGGGGSS",
	"GGGGGGGGGGGGGGSS",
	"DDDDDDDDDDDDDDSS",
	"DDDDDDDDDDDDDDSS",
	"DDDDDDDDDDDDDDSS",
	"DDDDDDDDDDDDDDSS",
	"DDDDDDDDDDDDDDSS",
	"DDDDDDDDDDDDDDSS",
	"DDDDDDDDDDDDDDSS",
	"DDDDDDDDDDDDDDSS",
	"ddddddddddddddSS",
	"..SSSSSSSSSSSSSS",
	"..SSSSSSSSSSSSSS",
]

# Wordmark letterforms, drawn for this file. Rows 0-6 are cap height,
# rows 7-8 hold descenders.
GLYPHS = {
	"B": ["####.", "#...#", "#...#", "####.", "#...#", "#...#", "####.", ".....", "....."],
	"l": ["##.", ".#.", ".#.", ".#.", ".#.", ".#.", "###", "...", "..."],
	"o": [".....", ".....", ".###.", "#...#", "#...#", "#...#", ".###.", ".....", "....."],
	"c": [".....", ".....", ".####", "#....", "#....", "#....", ".####", ".....", "....."],
	"k": ["#...", "#...", "#..#", "#.#.", "##..", "#.#.", "#..#", "....", "...."],
	"y": [".....", ".....", "#...#", "#...#", "#...#", ".####", "....#", "....#", ".###."],
	"O": [".###.", "#...#", "#...#", "#...#", "#...#", "#...#", ".###.", ".....", "....."],
	"S": [".####", "#....", "#....", ".###.", "....#", "....#", "####.", ".....", "....."],
}

# Small capitals for the "PLACEHOLDER" tag.
TAG_GLYPHS = {
	"P": ["##.", "#.#", "##.", "#..", "#.."],
	"L": ["#..", "#..", "#..", "#..", "###"],
	"A": [".#.", "#.#", "###", "#.#", "#.#"],
	"C": [".##", "#..", "#..", "#..", ".##"],
	"E": ["###", "#..", "##.", "#..", "###"],
	"H": ["#.#", "#.#", "###", "#.#", "#.#"],
	"O": [".#.", "#.#", "#.#", "#.#", ".#."],
	"D": ["##.", "#.#", "#.#", "#.#", "##."],
	"R": ["##.", "#.#", "##.", "#.#", "#.#"],
}


class Canvas:
	def __init__(self, width, height):
		self.width = width
		self.height = height
		self.px = [["."] * width for _ in range(height)]

	def stamp(self, grid, x0, y0, scale, color=None):
		"""Draw a character grid; '.' is skipped. color overrides the grid's keys."""
		for gy, row in enumerate(grid):
			for gx, key in enumerate(row):
				if key == ".":
					continue
				for y in range(y0 + gy * scale, y0 + (gy + 1) * scale):
					for x in range(x0 + gx * scale, x0 + (gx + 1) * scale):
						self.px[y][x] = color or key


def text_grid(text, glyphs, rows):
	"""Lay glyphs out left to right with one empty column between them."""
	out = [""] * rows
	for i, ch in enumerate(text):
		for r in range(rows):
			out[r] += ("." if i else "") + glyphs[ch][r]
	return out


def draw_icon(size):
	scale = size // len(BLOCK)
	assert scale * len(BLOCK) == size
	canvas = Canvas(size, size)
	canvas.stamp(BLOCK, 0, 0, scale)
	return canvas


def draw_header():
	width, height = 672, 72  # same size as the upstream header
	block_scale, word_scale, tag_scale = 4, 6, 2
	word = text_grid("BlockyOS", GLYPHS, 9)
	tag = text_grid("PLACEHOLDER", TAG_GLYPHS, 5)

	block_w = len(BLOCK[0]) * block_scale
	word_w = len(word[0]) * word_scale
	tag_w = len(tag[0]) * tag_scale
	gap = 20
	total = block_w + gap + word_w + word_scale + gap + tag_w
	x = (width - total) // 2

	canvas = Canvas(width, height)
	canvas.stamp(BLOCK, x, 4, block_scale)
	x += block_w + gap
	word_y = 6
	canvas.stamp(word, x + word_scale, word_y + word_scale, word_scale, "w")
	canvas.stamp(word, x, word_y, word_scale, "W")
	x += word_w + word_scale + gap
	# Sit the tag on the wordmark baseline.
	baseline = word_y + 7 * word_scale
	canvas.stamp(tag, x, baseline - 5 * tag_scale, tag_scale, "t")
	return canvas


# --- PNG encoding -------------------------------------------------------------

def _chunk(kind, data):
	body = kind + data
	return struct.pack(">I", len(data)) + body + struct.pack(">I", zlib.crc32(body) & 0xffffffff)


def _paeth(a, b, c):
	p = a + b - c
	pa, pb, pc = abs(p - a), abs(p - b), abs(p - c)
	if pa <= pb and pa <= pc:
		return a
	return b if pb <= pc else c


def _filter_row(kind, row, prev):
	out = bytearray(len(row))
	for i, v in enumerate(row):
		a = row[i - 1] if i else 0  # filters work per byte here (bpp < 8)
		b = prev[i]
		c = prev[i - 1] if i else 0
		if kind == 0:
			pred = 0
		elif kind == 1:
			pred = a
		elif kind == 2:
			pred = b
		elif kind == 3:
			pred = (a + b) // 2
		else:
			pred = _paeth(a, b, c)
		out[i] = (v - pred) & 0xff
	return out


def _filtered(rows, choice):
	prev = bytes(len(rows[0]))
	out = bytearray()
	for row in rows:
		if choice == "adaptive":
			cands = [(k, _filter_row(k, row, prev)) for k in range(5)]
			kind, data = min(cands, key=lambda kd: sum(v if v < 128 else 256 - v for v in kd[1]))
		else:
			kind, data = choice, _filter_row(choice, row, prev)
		out.append(kind)
		out += data
		prev = row
	return bytes(out)


def encode_png(canvas):
	used = sorted({k for row in canvas.px for k in row}, key=lambda k: (COLORS[k] is not None, k))
	index = {k: i for i, k in enumerate(used)}
	depth = next(d for d in (1, 2, 4, 8) if len(used) <= 1 << d)
	per_byte = 8 // depth

	rows = []
	for line in canvas.px:
		packed = bytearray()
		for start in range(0, canvas.width, per_byte):
			byte = 0
			for j in range(per_byte):
				i = start + j
				byte = (byte << depth) | (index[line[i]] if i < canvas.width else 0)
			packed.append(byte)
		rows.append(bytes(packed))

	best = None
	for choice in (0, 1, 2, 3, 4, "adaptive"):
		raw = _filtered(rows, choice)
		for strategy in (zlib.Z_DEFAULT_STRATEGY, zlib.Z_FILTERED, zlib.Z_RLE):
			for level in range(1, 10):
				comp = zlib.compressobj(level, zlib.DEFLATED, 15, 9, strategy)
				data = comp.compress(raw) + comp.flush()
				if best is None or len(data) < len(best):
					best = data

	palette = b"".join(bytes(COLORS[k] or (0, 0, 0)) for k in used)
	png = b"\x89PNG\r\n\x1a\n"
	png += _chunk(b"IHDR", struct.pack(">IIBBBBB", canvas.width, canvas.height, depth, 3, 0, 0, 0))
	png += _chunk(b"PLTE", palette)
	if COLORS[used[0]] is None:
		png += _chunk(b"tRNS", b"\x00")  # only index 0 is transparent
	png += _chunk(b"IDAT", best)
	png += _chunk(b"IEND", b"")
	return png


def encode_svg(canvas):
	"""Scalable icon: one crisp rect per horizontal run of equal pixels."""
	parts = [
		'<?xml version="1.0" encoding="UTF-8"?>\n',
		"<!-- SPDX-License-Identifier: MIT -->\n",
		"<!-- Copyright (c) 2026 BlockyOS contributors -->\n",
		"<!-- BlockyOS placeholder icon, generated by util/blockyos/make_placeholder_art.py -->\n",
		'<svg xmlns="http://www.w3.org/2000/svg" width="48" height="48" viewBox="0 0 %d %d" '
		'shape-rendering="crispEdges">\n' % (canvas.width, canvas.height),
	]
	for y, line in enumerate(canvas.px):
		x = 0
		while x < canvas.width:
			key = line[x]
			end = x
			while end < canvas.width and line[end] == key:
				end += 1
			if COLORS[key] is not None:
				parts.append('<rect x="%d" y="%d" width="%d" height="1" fill="#%02x%02x%02x"/>\n'
						% ((x, y, end - x) + COLORS[key]))
			x = end
	parts.append("</svg>\n")
	return "".join(parts).encode("ascii")


OUTPUTS = {
	"textures/base/pack/logo.png": lambda: encode_png(draw_icon(256)),
	"textures/base/pack/menu_header.png": lambda: encode_png(draw_header()),
	"misc/luanti-xorg-icon-128.png": lambda: encode_png(draw_icon(128)),
	"misc/luanti.svg": lambda: encode_svg(draw_icon(16)),
}


def main():
	for rel, make in OUTPUTS.items():
		data = make()
		with open(os.path.join(REPO, rel), "wb") as f:
			f.write(data)
		print("wrote %s (%d bytes)" % (rel, len(data)))


if __name__ == "__main__":
	main()
