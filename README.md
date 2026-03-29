# open-look

OpenLook and XView codebase modernized for Linux with CMake.

## Release status

This repository is the initial release preview of the full package:
`olwm`, `olgx`, `xview`, public headers, and runtime resources.

- Preview quality: not tested in depth yet.
- Target release date: April 1, 2026.
- Current state: usable for early testing, integration, and porting work.

## What is in the package

Source tree:

- `src/olwm`: Open Look window manager sources (`olwm`)
- `lib/olgx`: OPEN LOOK graphics library (`libolgx`)
- `lib/libxview`: XView toolkit library (`libxview`)
- `include/olgx`, `include/pixrect`, `include/xview`,
  `include/xview_private`: exported/public-style headers
- `resources/images`, `resources/bitmaps`, `resources/fonts`:
  runtime assets
- `config`: default menus and app-defaults
- `docs/man/olwm.man`: manual page source

Build/staging layout:

All build intermediates stay in `build/`.
Final staged output is assembled under `bin/` as an `OPENWINHOME` tree.

- `bin/bin`: executables (currently `olwm`)
- `bin/lib`: shared/static libs and runtime menu/config files
- `bin/include`: headers plus runtime `images` and `bitmaps`
- `bin/share/man`: man pages
- `bin/share/fonts/openlook`: packaged font files

## Build

Requirements:

- C compiler with C99 support
- CMake 3.16+
- X11 development libraries:
  - `libX11`
  - `libXext` (used by `olwm`)
- `libtirpc` development library (used by XView selection/RPC code)
- Standard C math library (`libm`, usually provided by libc toolchain)

Recommended package install commands:

Debian/Ubuntu:

```bash
sudo apt update
sudo apt install -y build-essential cmake libx11-dev libxext-dev libtirpc-dev xutils-dev
```

Fedora/RHEL:

```bash
sudo dnf install -y gcc gcc-c++ make cmake libX11-devel libXext-devel libtirpc-devel xorg-x11-font-utils
```

Arch Linux:

```bash
sudo pacman -S --needed base-devel cmake libx11 libxext libtirpc xorg-mkfontscale xorg-mkfontdir
```

Build commands:

```bash
cmake -S . -B build
cmake --build build -j"$(nproc)"
```

## Installation

This preview supports two installation models.

1. Staged local install in this repository
2. System install under a prefix (for example `/usr/local`)

### 1) Staged local install (recommended for preview testing)

After build, everything is staged in `bin/` as an `OPENWINHOME` tree.

```bash
source ./bin/use-openwinhome.sh
```

This sets:

- `OPENWINHOME=<repo>/bin`
- `PATH=$OPENWINHOME/bin:$PATH`
- `LD_LIBRARY_PATH=$OPENWINHOME/lib:$LD_LIBRARY_PATH`

### 2) System install with CMake

```bash
cmake --install build --prefix /usr/local
sudo ldconfig
```

If you use a different prefix, set `OPENWINHOME` to that prefix when running.

## Font installation (important)

XView/Open Look applications require Open Look fonts to be visible in the
X server font path. If fonts are missing you may get `BadFont` failures.

Install the packaged BDF font sets:

```bash
sudo mkdir -p /usr/local/share/fonts/openlook/bdf/{misc,75dpi,100dpi}
sudo cp -a bin/share/fonts/openlook/bdf/. /usr/local/share/fonts/openlook/bdf/
sudo mkfontdir /usr/local/share/fonts/openlook/bdf/misc
sudo mkfontdir /usr/local/share/fonts/openlook/bdf/75dpi
sudo mkfontdir /usr/local/share/fonts/openlook/bdf/100dpi
```

Enable them for the current X session:

```bash
xset +fp /usr/local/share/fonts/openlook/bdf/misc
xset +fp /usr/local/share/fonts/openlook/bdf/75dpi
xset +fp /usr/local/share/fonts/openlook/bdf/100dpi
xset fp rehash
```

Quick check:

```bash
xlsfonts | grep -Ei 'open look|olglyph|olcursor|lucida'
```

If `mkfontdir` is missing, install your distro's X font utilities package.

## Runtime resources and defaults

The runtime package expects these assets to exist under `OPENWINHOME`:

- `lib/openwin-menu*`
- `lib/app-defaults/Olwm`
- `include/images`
- `include/bitmaps` and `include/X11/bitmaps`

For staged local runs (`OPENWINHOME=<repo>/bin`), these are already in place
after build.

## Running from the staged tree

Use the generated helper so runtime lookup paths point to `bin/`:

```bash
source ./bin/use-openwinhome.sh
```

Then launch, for example:

```bash
olwm
```

For nested testing with Xephyr:

```bash
Xephyr :2 -ac -screen 1024x768 &
DISPLAY=:2 xrdb -merge bin/lib/app-defaults/Olwm
DISPLAY=:2 olwm
```

## Notes

- Linux is the target platform in this port.
- Legacy non-Linux path branches have been reduced where possible.
- More validation and compatibility testing is still required before final
  release.

## Legal notice

NOTICE TO USER: The source code, including the glyphs or icons forming a
part of the OPEN LOOK TM Graphic User Interface, in these files is
copyrighted under U.S. and international laws.

(c) Copyright 1989 Sun Microsystems, Inc.
Sun design patents pending in the U.S. and foreign countries.
OPEN LOOK is a trademark of AT&T.

This source code is provided "as is" without express or implied warranty.
