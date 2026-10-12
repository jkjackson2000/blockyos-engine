<!-- SPDX-License-Identifier: MIT -->
<!-- Copyright (c) 2026 BlockyOS contributors -->

# BlockyOS engine notices

**BlockyOS engine, based on Luanti 5.17.0.**

This repository (`jkjackson2000/blockyos-engine`, branch `blockyos/5.17`) is a
modified version of [Luanti](https://www.luanti.org/), the open-source voxel
game engine. BlockyOS uses it to run the BlockyOS world, which is the shell of
the BlockyOS Linux system (<https://github.com/jkjackson2000/blockyos>).
Luanti is developed by the Luanti contributors; BlockyOS is not affiliated
with or endorsed by the Luanti project.

The upstream `README.md` and `LICENSE.txt` are kept unchanged. This file
covers what BlockyOS changed and how each component is licensed.

## Upstream base

| | |
|---|---|
| Project | Luanti, <https://github.com/luanti-org/luanti> |
| Tag | `5.17.0` |
| Commit | `c0e6812b1a4260bb25a1f606f70f55f4962bb97d` |
| Upstream credits | `builtin/mainmenu/credits.json` (shown in the About tab), source file headers, and the git history |

## Corresponding source

The complete source of every BlockyOS engine build, including these
modifications, is published at <https://github.com/jkjackson2000/blockyos-engine>.
Each BlockyOS release pins the exact engine commit in the BlockyOS version
manifest. To see every change against upstream:

    git diff c0e6812b1a4260bb25a1f606f70f55f4962bb97d

Build instructions are in `doc/compiling/`. Source obligations for the whole
BlockyOS OS image are tracked in jkjackson2000/blockyos#6.

## Licenses

License facts below come from the files at tag `5.17.0`. Per-file headers
are authoritative where they differ from this summary. This is a summary,
not legal advice.

| Component | Path | License | License text |
|---|---|---|---|
| Engine | `src/` | LGPL-2.1-or-later | `COPYING.LESSER`, `LICENSE.txt` |
| Engine, permissive files | e.g. `src/util/srp.*` (MIT), `src/util/sha1.*` (MIT), `src/util/ieee_float.cpp` (MIT), `src/threading/*` (JThread, MIT), `src/noise.*` (BSD-2-Clause), `src/gui/guiOpenURL.*`, `src/util/colorize.*` and other ISC files, `src/util/base64.*` and `src/irrlicht_changes/CGUITTFont.*` (zlib-style), Irrlicht-derived `src/gui/*` files (zlib) | MIT / BSD / ISC / zlib, as stated in each file | File headers |
| Builtin Lua | `builtin/` | LGPL-2.1-or-later | `COPYING.LESSER` |
| Shaders | `client/shaders/` | LGPL-2.1-or-later | `COPYING.LESSER` |
| Translations | `po/` | No per-file header; distributed as part of the Luanti source (LGPL-2.1-or-later) | `COPYING.LESSER` |
| IrrlichtMt (Luanti's Irrlicht fork) | `irr/` | zlib (Irrlicht Engine License); vendored GL headers in `irr/src/vendor/` are MIT | `irr/LICENSE`, `LICENSE.txt` |
| Base textures | `textures/base/pack/` | CC BY-SA 3.0 by default; CC BY-SA 4.0 (Zughy, cx384, DS); Apache-2.0 (`refresh.png`; Material Design `contentdb_*.png`); CC BY 4.0 (`server_public.png`, Twemoji) | `LICENSE.txt` (authors and URIs), `LICENSES/Apache-2.0.txt` |
| Fonts: Arimo, Cousine | `fonts/` | See the mismatch note below | `fonts/Arimo-LICENSE.txt`, `fonts/Cousine-LICENSE.txt` (OFL-1.1), `LICENSES/Apache-2.0.txt` |
| Font: DroidSansFallbackFull | `fonts/` | Apache-2.0 | `fonts/DroidSansFallbackFull-LICENSE.txt`, `LICENSES/Apache-2.0.txt` |
| Lua 5.1.5, patched (used when LuaJIT is not used) | `lib/lua/` | MIT | `lib/lua/COPYRIGHT`, `LICENSE.txt` |
| LuaBitOp (used with bundled PUC Lua only) | `lib/bitop/` | MIT (Mike Pall) | File headers |
| JsonCpp (used when `ENABLE_SYSTEM_JSONCPP` is off or no system copy is found) | `lib/jsoncpp/` | Public domain, or MIT where public domain is not recognized | `lib/jsoncpp/jsoncpp.cpp` header |
| mini-gmp (used when `ENABLE_SYSTEM_GMP` is off or no system GMP is found) | `lib/gmp/` | LGPL-3.0-or-later OR GPL-2.0-or-later | `lib/gmp/mini-gmp.h` header |
| lstrpack | `lib/lstrpack/` | MIT | `lib/lstrpack/LICENSE.txt` |
| tiniergltf | `lib/tiniergltf/` | MIT (Lars Müller) | `lib/tiniergltf/Readme.md` |
| Catch2 (linked when `BUILD_UNITTESTS` or `BUILD_BENCHMARKS` is on; `BUILD_UNITTESTS` defaults to on) | `lib/catch2/` | BSL-1.0 | File headers |
| SHA-224/SHA-256 | `lib/sha256/` | MIT, BlockyOS contributors (replaces upstream code, see below) | `lib/sha256/my_sha256.h` |
| Development Test game | `games/devtest/` | Same as Luanti (`games/devtest/LICENSE.txt`) | `LICENSE.txt` |
| BlockyOS placeholder art and its generator | see "Changes from upstream" | MIT, BlockyOS contributors | `util/blockyos/make_placeholder_art.py` header |

Notes:

- **Irrlicht / IJG acknowledgement.** IrrlichtMt is based in part on the work
  of the Independent JPEG Group: *this software is based in part on the work
  of the Independent JPEG Group.* It also uses zlib and libpng. BlockyOS has
  not modified `irr/`.
- **Arimo/Cousine license text mismatch.** `LICENSE.txt` lists Arimo and
  Cousine as Apache License 2.0 ("Digitized data copyright (c) 2010-2012
  Google Corporation"), but `fonts/Arimo-LICENSE.txt` and
  `fonts/Cousine-LICENSE.txt` carry the SIL Open Font License 1.1 ("Copyright
  2018 The Noto Project Authors"). Until that is resolved upstream, BlockyOS
  ships both texts: the OFL-1.1 files install with the fonts, and
  `LICENSES/Apache-2.0.txt` installs with the documentation.
- `LICENSE.txt` also lists Bitstream Vera fonts; no Vera font files are
  present at this tag.
- **Development Test** (`games/devtest/`) is an engine test rig. The BlockyOS
  product does not install it (`INSTALL_DEVTEST` stays off).
- `LICENSE.txt` credits erle for `misc/luanti.svg` and
  `textures/base/pack/logo.png`, and grorp for
  `textures/base/pack/menu_header.png`. Those credits describe the upstream
  art. BlockyOS replaced those three files and `misc/luanti-xorg-icon-128.png`
  with its own placeholders, listed below.

## SHA-256 replacement

Upstream `lib/sha256/` carried an OpenSSL-derived SHA-256 under the original
SSLeay/OpenSSL license, including its advertising clause. The engine always
links it (SRP login in `src/util/srp.cpp`), and `src/util/hashing.cpp` uses it
when `ENABLE_OPENSSL` is off.

BlockyOS replaced it with an original SHA-224/SHA-256 implementation written
from NIST FIPS 180-4 (MIT, BlockyOS contributors). It keeps the API callers
use (`SHA224_*`/`SHA256_*` `Init`/`Update`/`Final`, the one-shot `SHA224()` and
`SHA256()`, `SHA256_CTX`, `SHA224_DIGEST_LENGTH`, `SHA256_DIGEST_LENGTH`), so
no caller changed. The header maps those names to `blockyos_sha*` link
symbols so they cannot collide with OpenSSL's libcrypto exports. The unused
`SHA256_Transform()` was dropped. The `ENABLE_OPENSSL` path is unchanged.
`src/unittest/test_sha256.cpp` checks the implementation against FIPS 180-4
example vectors.

## Changes from upstream

Every file BlockyOS changed, added, or removed relative to commit
`c0e6812b1a4260bb25a1f606f70f55f4962bb97d`. Modified text files also carry a
"Modified for BlockyOS" notice near the top.

Modified:

| File | Change |
|---|---|
| `CMakeLists.txt` | `PROJECT_NAME_CAPITALIZED` is `BlockyOS` (window caption, `--version`, in-game version text, pause-menu text, `core.get_version().project`, HTTP user agent). `ENABLE_UPDATE_CHECKER` defaults to off. Installs this file, `LICENSE.txt`, `COPYING.LESSER`, and `LICENSES/Apache-2.0.txt` with the documentation. |
| `builtin/mainmenu/tab_about.lua` | About tab shows BlockyOS, "Based on Luanti 5.17.0", a source code link to this repository, and an introduction above the unchanged upstream credits. |
| `builtin/mainmenu/tab_local.lua` | The no-game notice names BlockyOS instead of describing Luanti as the product. |
| `lib/sha256/CMakeLists.txt` | Builds the new implementation; drops the `endian.h` check. |
| `lib/sha256/my_sha256.h` | Replaced with original MIT code (see above). |
| `lib/sha256/sha256.c` | Replaced with original MIT code (see above). |
| `src/unittest/CMakeLists.txt` | Registers `test_sha256.cpp`. |
| `misc/org.luanti.luanti.desktop` | Visible `Name` and `GenericName` are BlockyOS. |
| `misc/org.luanti.luanti.metainfo.xml` | Visible `<name>` is BlockyOS. |
| `misc/luanti.svg` | Replaced with the BlockyOS placeholder icon (generated). |

Modified binary files (cannot carry a notice; all generated by
`util/blockyos/make_placeholder_art.py`, MIT, BlockyOS contributors):

| File | Change |
|---|---|
| `textures/base/pack/logo.png` | BlockyOS placeholder block, 256x256 (About tab and window icon). |
| `textures/base/pack/menu_header.png` | BlockyOS placeholder block and wordmark, 672x72 (main menu header). |
| `misc/luanti-xorg-icon-128.png` | BlockyOS placeholder block, 128x128 (installed as the hicolor 128x128 `luanti.png` icon). |

Added:

| File | Purpose |
|---|---|
| `.github/workflows/blockyos.yml` | BlockyOS x86_64 Linux CI: build with and without OpenSSL, unit tests, devtest integration and software-rendered singleplayer tests. Upstream workflows are left in place but not enabled on this fork. |
| `BLOCKYOS_NOTICES.md` | This file. |
| `LICENSES/Apache-2.0.txt` | Apache License 2.0 text (from <https://www.apache.org/licenses/LICENSE-2.0.txt>), for the Apache-licensed fonts and textures. |
| `src/unittest/test_sha256.cpp` | Unit test for `lib/sha256`. |
| `util/blockyos/make_placeholder_art.py` | Generates the placeholder art from hand-placed pixel grids (Python 3 standard library only). |

Removed:

| File | Reason |
|---|---|
| `lib/sha256/cmake_config.h.in` | Only held the `endian.h` check the old code needed. |

Deliberately unchanged: the lowercase engine name `luanti` (binary, data and
user directories, gettext domain, install paths, desktop and icon IDs), the
network protocol, and Lua API names. These are engine identifiers, and
keeping them eases upstream merges. Windows, macOS, and Android packaging
files (`.ico`, `.icns`, `android/`, `fastlane/`) are not BlockyOS targets and
are untouched.
