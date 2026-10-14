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
- `apps`: accessories (`cmdtool`, `textedit`, `clock`, `props`,
  `xtoolplaces`, `hello`, and `olwmslave`)
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

- `bin/bin`: window manager and accessory executables
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
- `libtirpc` development library (used by XView selection/RPC code).
  Modern glibc no longer ships `<rpc/rpc.h>` at the top level; headers
  live under `/usr/include/tirpc` and CMake adds that include path.
- Boost
- Standard C math library (`libm`, usually provided by libc toolchain)
- Terminfo compiler (`tic`, provided by ncurses)

Recommended package install commands:

Debian/Ubuntu:

```bash
sudo apt update
sudo apt install -y build-essential cmake libx11-dev libxext-dev libtirpc-dev xutils-dev ncurses-bin libboost-dev
```

Fedora/RHEL:

```bash
sudo dnf install -y gcc gcc-c++ make cmake libX11-devel libXext-devel libtirpc-devel xorg-x11-font-utils ncurses boost-devel
```

Arch Linux:

```bash
sudo pacman -S --needed base-devel cmake libx11 libxext libtirpc xorg-mkfontscale xorg-mkfontdir ncurses boost-libs
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
cmake -S . -B build -DCMAKE_INSTALL_PREFIX=/usr/local
cmake --build build -j"$(nproc)"
sudo cmake --install build
sudo ldconfig
```

Set `CMAKE_INSTALL_PREFIX` when configuring to choose another prefix. Programs
use this prefix when `OPENWINHOME` is unset. `olwm` also exports that default
to menu-launched applications, including in display-manager sessions.
If you override the prefix only with `cmake --install --prefix`, set
`OPENWINHOME` to that prefix when running.
The window manager and all accessories are installed in `<prefix>/bin`.

### SDK installation and application builds

The `SDK` component installs both shared and static XView/olgx libraries,
all headers under `include/` (including Pixrect compatibility and private
headers), and relocatable `pkg-config` files. It is also included in a full
installation. To install only the SDK after building:

```bash
sudo cmake --install build --component SDK
sudo ldconfig
gcc quit.c $(pkg-config --cflags --libs xview) -o quit_prog
```

For a custom prefix, add `<prefix>/lib/pkgconfig` to `PKG_CONFIG_PATH`.
The flags include the libtirpc header path required by selection-service
headers and enable the BSD types used by XView in strict C modes.
X11, libtirpc and Boost development headers remain external SDK
prerequisites; `pkg-config` is needed for the command above.

Check SDK installation completeness without modifying the system:

```bash
cmake --build build --target sdk_install_check
```

This installs into a temporary prefix, compares every header with its source,
preprocesses every installed header with and without `OW_I18N`, and compiles
and links an independent XView client using installed `pkg-config` flags.
Historical Sun framebuffer declarations requiring SunOS interfaces are guarded
on Linux. Installing their headers does not provide Sun framebuffer drivers
or a complete Pixrect implementation. The `OW_I18N` checks validate header
dependencies; they do not enable internationalization in the built libraries.

`cmdtool` uses `TERM=sun-cmd`. The build compiles its terminal description
into `share/terminfo` in both the staged tree and the installed prefix.
`cmdtool` adds `$OPENWINHOME/share/terminfo` to `TERMINFO_DIRS` for its child
processes, preserving existing search paths and the system defaults. This
allows Vim, `clear`, and other terminfo applications to recognize `sun-cmd`
even when the host's terminal database omits it.

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

The default workspace menu search checks `$HOME/.openwin-menu`, then
`/etc/X11/olwm/openwin-menu`, then `$OPENWINHOME/lib/openwin-menu`
(with additional locale-specific paths in internationalized builds).
Installed programs do not require the source tree. Source paths may still
appear in debugging information; these are separate from runtime lookups.
On compilers supporting `-fmacro-prefix-map`, assertion messages use relative
source filenames even in Debug builds. Full paths remain in debug information
for IDE source debugging. `cmake --install build --strip` removes debug
information from installed binaries while keeping the local build debuggable.
Absolute paths in `build/CMakeCache.txt` are normal build metadata and are
not installed runtime configuration.

## Running from the staged tree

In VS Code, select **Debug workspace (OpenLook)** and press F5 to build
the package and debug `olwm` in a nested Xephyr session on `:2`.
The Programs menu launches Command Tool, Clock, Text Editor, Properties,
and Hello. The Workspace menu also offers Refresh, Restart WM, Reread Menu
File, Save Window Layout (`xtoolplaces`, writing to `~/.xtoolplaces`), Exit WM,
and Exit, with separators between groups. `olwmslave` is a window-manager
helper and has no standalone menu entry.
At desktop startup, `olwm` runs a readable `~/.xtoolplaces` with `/bin/sh`
after initializing the window manager. Saved commands can find tools in
`$OPENWINHOME/bin`, and the file does not need executable permission.
Restart WM preserves the running tools without replaying the saved layout.
**Debug textedit (OpenLook)** starts the editor automatically.

Compiled window-manager font defaults match `config/Xdefaults`: medium
Helvetica for menu items and bold Helvetica for titles. User X resources
can override these, and the fonts must be available in the X server font path.

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

`props` supports its colour palette on 8-bit PseudoColor and 16/24-bit
TrueColor displays. Grayscale displays omit the Colours category.
For automated palette, preview, Apply/Reset, and grayscale checks (requires
Xvfb):

```bash
cmake --build build --target props_color_test
python3 tests/run-props-colors.py
```

- Linux is the target platform in this port.
- Legacy non-Linux path branches have been reduced where possible.
- More validation and compatibility testing is still required before final
  release.

## Archived CD-ROM compatibility fixtures

The optional programs under `tests/ian-darwin/` build against this repository's
SDK. Enable them and attempt every target with:

```bash
cmake -S . -B build -DOPENLOOK_BUILD_IAN_DARWIN=ON
python3 tests/ian-darwin/build-all.py --label current
```

Build individual programs with, for example,
`cmake --build build --target ian_coffee`. Executables stage under
`bin/bin/ian-darwin/`. The runner records per-target logs and returns nonzero
when any target fails. See [docs/IAN.md](docs/IAN.md) for the source inventory,
fixes, remaining failures, and validation commands.

## Legal notice

NOTICE TO USER: The source code, including the glyphs or icons forming a
part of the OPEN LOOK TM Graphic User Interface, in these files is
copyrighted under U.S. and international laws.

(c) Copyright 1989 Sun Microsystems, Inc.
Sun design patents pending in the U.S. and foreign countries.
OPEN LOOK is a trademark of AT&T.

This source code is provided "as is" without express or implied warranty.
