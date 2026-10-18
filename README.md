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
- `apps`: File Manager (`filemgr`) and accessories (`cmdtool`, `textedit`,
  `clock`, `props`, `xtoolplaces`, and `olwmslave`)
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
  - `libXpm` (used by File Manager)
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
sudo apt install -y build-essential cmake libx11-dev libxext-dev libxpm-dev libtirpc-dev xutils-dev ncurses-bin libboost-dev
```

Fedora/RHEL:

```bash
sudo dnf install -y gcc gcc-c++ make cmake libX11-devel libXext-devel libXpm-devel libtirpc-devel xorg-x11-font-utils ncurses boost-devel
```

Arch Linux:

```bash
sudo pacman -S --needed base-devel cmake libx11 libxext libxpm libtirpc xorg-mkfontscale xorg-mkfontdir ncurses boost-libs
```


Build commands:

```bash
cmake -S . -B build
cmake --build build -j"$(nproc)"
```

### Build and debug with Docker

The optional Docker configuration uses
`wischner/gcc-x86_64-linux-x11:latest` for the compiler, CMake, development
libraries, GDB, X11 tools, and Xephyr. No host compiler, development libraries,
GDB, or Xephyr installation is needed. The host needs Bash and a running
Docker Engine accessible to your user. Docker pulls the image on first use
if it is missing locally.

To compile without installing the native build dependencies:

```bash
bash .vscode/openlook-docker.sh build
```

In VS Code, select **Tasks: Run Task → build (Docker)**, or select
**Debug workspace (OpenLook Docker)** and press F5 to build and debug only
`olwm`. Debugging requires the VS Code C/C++ extension and a local Linux
X11/XWayland desktop with `DISPLAY` and its X authority credentials available
to VS Code. Xephyr runs inside Docker and displays a window on that desktop;
compilation alone does not require a display. The native debug profile remains
available separately.

Docker uses `build/docker/` for intermediates and `bin/docker/` for its
OPENWINHOME tree, separate from the native build. The repository is mounted
at the same absolute path inside the container so source breakpoints resolve.
Builds run as your host user to preserve file ownership. The debugger uses
container GDB through `docker exec`; no host GDB is needed. The debug
container receives `SYS_PTRACE` for debugging and access to the host X socket
and authority file for its Xephyr window.

Stopping debugging removes the dedicated container and its processes. If a
session is interrupted, use **openlook: stop Docker session** or:

```bash
bash .vscode/openlook-docker.sh stop
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

In VS Code, use **Run Build Task** (Ctrl+Shift+B) to build the package.
Select **Debug workspace (OpenLook)** and press F5 to debug `olwm` in a
nested Xephyr session on `:2`. Launch applications from its Programs menu.
The Programs menu launches File Manager, Command Tool, Clock, Text Editor,
and Properties. The Workspace menu also offers Refresh, Restart WM, Reread Menu
File, Save Window Layout (`xtoolplaces`, writing to `~/.xtoolplaces`), Exit WM,
and Exit, with separators between groups. `olwmslave` is a window-manager
helper and has no standalone menu entry.

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

## File Manager

**Programs → File Manager** launches the original Sun DeskSet File Manager
V3.6, ported from the Solaris 2.6 source archive. It is built and installed as
`filemgr` alongside the other applications. The menu uses its original `-C`
mode, with generic folder/document/application icons instead of the Classing
Engine. Source provenance and retained notices are described in
`apps/filemgr/UPSTREAM.txt`.

```bash
filemgr -C -d /path/to/folder
```

The port supports browsing, folder/document creation, copying, moving, and
renaming. Solaris volume management, removable-media formatting/ejecting, the
Classing Engine, and Solaris quota RPC integration are unavailable. Free space
comes from Linux `statvfs`. Printing and custom commands require the external
programs configured for those actions. In Docker, the file manager sees the
container filesystem and the mounted repository.

The Docker image already provides the required `libXpm` headers and library.
To run the isolated Xephyr operation and normal-shutdown smoke test after
building:

```bash
bash tests/run-filemgr-smoke.sh
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

## Legal notice

NOTICE TO USER: The source code, including the glyphs or icons forming a
part of the OPEN LOOK TM Graphic User Interface, in these files is
copyrighted under U.S. and international laws.

(c) Copyright 1989 Sun Microsystems, Inc.
Sun design patents pending in the U.S. and foreign countries.
OPEN LOOK is a trademark of AT&T.

This source code is provided "as is" without express or implied warranty.
