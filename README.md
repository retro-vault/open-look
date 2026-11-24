# open-look

OpenLook and XView codebase modernized for Linux with CMake.

## Release status

This repository is the initial release preview of the full package:
`olwm`, `olgx`, `xview`, public headers, and runtime resources.

- Preview quality: not tested in depth yet.
- Original target release date: April 1, 2026; this repository still identifies
  itself as a preview.
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
- `docs/man`: window manager and accessory manual page sources
- [C coding standards](docs/standards/C-CODING-STYLE.md): development rules

Build/staging layout:

All build intermediates stay in `build/`.
Final staged output is assembled under `bin/` as an `OPENWINHOME` tree.

- `bin/bin`: window manager and accessory executables
- `bin/lib`: shared/static libs and runtime menu/config files
- `bin/include`: headers plus runtime `images` and `bitmaps`
- `bin/share/man`: man pages
- `bin/share/fonts/openlook/bdf/{misc,75dpi,100dpi}`: BDF fonts, aliases,
  and generated `fonts.dir` indexes
- `bin/share/locale`: Properties locale resources
- `bin/share/terminfo`: compiled `sun-cmd` terminal description
- `bin/use-openwinhome.sh`: Bash environment and X session setup helper

## Build

Requirements:

- C compiler with C99 support
- CMake 3.16+
- X11 development libraries:
  - `libX11`
  - `libXext` (used by `olwm`)
  - `libXpm` (used by File Manager and the shared application icon theme)
  - `libXrender` (used by MosaiX's transparent toolbar icons)
- `libtirpc` development library (used by XView selection/RPC code).
  Modern glibc no longer ships `<rpc/rpc.h>` at the top level; headers
  live under `/usr/include/tirpc` and CMake adds that include path.
- Boost
- `libnsl-dev` development library for the public client interface library for NIS (Network Information Service / Yellow Pages) and NI
- `libXrandr` development library for xrandr to allow the capture of events
- `libxpm-dev` development library for libxpm, X11 pixmap library
- Standard C math library (`libm`, usually provided by libc toolchain)
- Terminfo compiler (`tic`, provided by ncurses)
- X11 font index generator (`mkfontdir`, required at configure time)

Recommended package install commands:

Debian/Ubuntu:

```bash
sudo apt update
sudo apt install -y build-essential cmake libx11-dev libxext-dev libxpm-dev libxrender-dev libtirpc-dev xutils-dev ncurses-bin libboost-dev libnsl-dev libxrandr-dev libXrandr-devel libXpm-devel


Fedora/RHEL
```bash
sudo dnf config-manager --set-enabled crb
sudo dnf install -y gcc gcc-c++ make cmake libX11-devel libXext-devel libXpm-devel libXrender-devel \
  libtirpc-devel mkfontscale mkfontdir xset fontconfig ncurses boost-devel git-core \ 
  libXrandr-devel libnsl2-devel libtirpc-devel rpcsvc-proto-devel quota-devel

```


Arch Linux:

```bash
sudo pacman -S --needed base-devel cmake libx11 libxext libxpm libxrender libtirpc xorg-mkfontscale \
  xorg-mkfontdir ncurses boost-libs libnsl libxrandr libxpm rpcsvc-proto xorg-xset xorg-xlsfonts

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

Docker uses `build/docker/` for intermediates and the shared `bin/` for its
OPENWINHOME tree, also used by native builds. Switching build environments
replaces the staged artifacts; keep using the matching build and debug profile.
F5 builds the full package and populates `bin/bin`, `bin/lib`, `bin/include`,
and `bin/share`; the debugger then launches `bin/bin/olwm`. Docker does not
create a separate output directory inside `bin/`. The repository is mounted
at the same absolute path inside the container so source breakpoints resolve.
Builds run as your host user to preserve file ownership. The debugger uses
container GDB through `docker exec`; no host GDB is needed. The debug
container receives `SYS_PTRACE` for debugging and access to the host X socket
and authority file for its Xephyr window.

MosaiX is included in the full build. Docker builds use the pinned CEF
distribution under `build/docker/deps/cef/source`, downloading it on first
use and reusing it afterward. The Docker helper resets `MOSAIX_CEF_ROOT`
so paths from temporary containers cannot break F5. For native builds,
`-DMOSAIX_CEF_ROOT=/path/to/cef` selects an existing matching distribution;
`-DMOSAIX_CEF_ROOT=` restores automatic dependency management.

MosaiX uses a single compact toolbar with native square buttons, transparent
13×13 Tango icons (resized from the original 16×16 artwork), an unlabeled URL
field, and a Properties button at the right. Native image buttons are 21×21
pixels and the toolbar is 26 pixels high, with bottom padding balancing
OpenLook's title-bar inset. The image-button outline has squarer corners and
four pixels of inset around each icon to keep its border clear. Disabled tools
retain the normal border and fade only the inner image.
A plain click selects the complete URL for replacement; dragging still selects
part of it. The default homepage is `https://github.com/retro-vault/open-look/`;
saved homepage preferences take precedence.
Properties groups General, Privacy, Content, and browsing-data controls in a
compact native sheet; Reset restores the currently applied values. Dialogs open
centered on the browser. Icon provenance and the upstream public-domain notice
are in `apps/mosaix/icons/`.

Application icons use the artwork in `assets/icons/bw-theme/`: browser for
MosaiX, terminal for Cmdtool/Shelltool, text-editor for Text Editor, folder for
File Manager, waste for Wastebasket, and properties for Workspace Properties.
Clock keeps its original face in the open window and uses the supplied square
frame with live hour/minute hands only in its minimized icon. Digital mode and
the seconds/date options remain available. Its icon label shows local time in
24-hour `HH:MM` format. MosaiX uses the label `MosaiX`, and an unnamed Text Editor
document uses `No file`. Theme artwork is shifted up eight pixels within each
64px PNG to leave room for the native labels. Disk Tool retains its existing
icon because the theme has no match.
Regenerate the checked-in XPM files with `python3 tools/convert-app-icons.py`
(Pillow is needed only for regeneration). The build stages them in
`bin/share/openlook/icons/bw-theme/`; installation copies them to
`share/openlook/icons/bw-theme/`. The shared loader preserves opaque black and
white pixels and the separate transparency mask, and also supports colour art.

The Docker debug session sets `MOSAIX_DISABLE_SANDBOX=1` for MosaiX because
Docker's default seccomp policy blocks Chromium's namespace sandbox. This
disables Chromium's process sandbox inside the development container; use
this session for trusted development content. Native launches retain the
sandbox unless explicitly started with `--no-sandbox` or that environment
variable. Restart F5 after changing the Docker launch environment.

After building, run `bash tests/run-mosaix-smoke.sh` to check menu-style
startup, compact toolbar resizing, dialog centering, JavaScript rendering,
vertical/horizontal scrollbar dragging, wheel synchronization,
certificate database initialization, relaunch, and both
keyboard and window-manager shutdown in an isolated Xephyr session.
Logs are written under `build/docker/mosaix-smoke/`.

Stopping debugging removes the dedicated container and its processes. If a
session is interrupted, use **openlook: stop Docker session** or:

```bash
bash .vscode/openlook-docker.sh stop
```

## Compact menu panel fitting

`window_fit_height(panel)` and the height part of `window_fit(panel)` use a
compact bottom inset for horizontal panels containing only `PANEL_BUTTON`
items with attached `PANEL_ITEM_MENU` menus. With the default panel insets,
the height is the lowest button bottom plus two pixels: a single row is its
button height plus six pixels, matching DeskSet's compact menu bars. Wrapped
rows remain included. Applications using these standard fitting calls need
no private `scrunch_window()` helper.

Empty panels, ordinary command buttons, mixed forms, vertical layouts,
width fitting and explicit nonzero `WIN_FIT_HEIGHT` margins retain their
existing sizing. `PANEL_EXTRA_PAINT_HEIGHT` and horizontal scrollbar space
remain additive. Explicit `XV_HEIGHT` values are unchanged.

The change is in `libxview`, so applications must load the rebuilt shared
library (or relink if using the static library). Rebuilding a consumer against
an older SDK does not update the toolkit. Docker consumers must also update
their SDK/runtime image.

The `panel_fit_test` target exercises menu and non-menu fitting, wrapped rows,
explicit margins, extra paint space, scrollbar reservation and placement of
content immediately below a fitted panel. Build it in the same environment
as XView, then run it on a test X display with the packaged fonts:

```bash
cmake --build build/docker --target panel_fit_test
OPENWINHOME="$PWD/bin" LD_LIBRARY_PATH="$PWD/bin/lib" \
    build/docker/tests/panel_fit_test -scale medium
```

Repeat with `small`, `large`, and `extra_large` to check the font scales.

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

When `DISPLAY` is set, the helper also merges `lib/app-defaults/Olwm` with
`xrdb` and adds the three packaged font directories with `xset`, when those
tools are available. Source it after selecting the display you want to use.

### 2) System install with CMake

```bash
cmake -S . -B build -DCMAKE_INSTALL_PREFIX=/usr/local
cmake --build build -j"$(nproc)"
sudo cmake --install build
source /usr/local/use-openwinhome.sh
```

Set `CMAKE_INSTALL_PREFIX` when configuring to choose another prefix. Programs
use this prefix when `OPENWINHOME` is unset. `olwm` also exports that default
to menu-launched applications, including in display-manager sessions.
If you override the prefix only with `cmake --install --prefix`, set
`OPENWINHOME` to that prefix when running.
The window manager and all accessories are installed in `<prefix>/bin`.
Installation also includes shared/static libraries, all exported header trees,
images and bitmap compatibility paths, menus, application defaults, the text
extras menu, File Manager help, manual pages, locale resources, terminfo, fonts,
and `<prefix>/use-openwinhome.sh`. Build before installing: the font indexes
and terminfo are generated by the build. CMake install copies project files;
install the system dependencies listed above separately.

Installed programs locate the packaged libraries through a relative runtime
library path, so `ldconfig` is not required for these programs. Downstream
applications linking XView may need their own loader configuration.

To inspect an installation without writing to the system prefix:

```bash
DESTDIR=/tmp/openlook-package cmake --install build
```

With the default prefix, the files appear under
`/tmp/openlook-package/usr/local`. `DESTDIR` is a packaging root and does not
change the compiled default `OPENWINHOME`. Docker builds use the CMake install
rules in `build/docker`; run installation with the same container toolchain,
using a writable bind mount or `DESTDIR` to retain the resulting package.

Both the staged and installed `bin` directories include a relative
`shelltool -> cmdtool` symlink, which starts the terminal in shelltool mode.

`cmdtool` uses `TERM=sun-cmd`. The build compiles its terminal description
into `share/terminfo` in both the staged tree and the installed prefix.
`cmdtool` adds `$OPENWINHOME/share/terminfo` to `TERMINFO_DIRS` for its child
processes, preserving existing search paths and the system defaults. This
allows Vim, `clear`, and other terminfo applications to recognize `sun-cmd`
even when the host's terminal database omits it.

## Font installation (important)

XView/Open Look applications require Open Look fonts to be visible in the
X server font path. If fonts are missing you may get `BadFont` failures.

The build stages all packaged BDF font sets and generates their `fonts.dir`
indexes. CMake install includes these fonts, aliases, and indexes automatically.
To enable them on the current X server, source the helper from your installation:

```bash
source /usr/local/use-openwinhome.sh
```

For a local build, use `source ./bin/use-openwinhome.sh`. Source the helper
in your X session startup to enable the fonts in future sessions. The Docker
preview configures its Xephyr font path automatically.

Quick check:

```bash
xlsfonts | grep -Ei 'open look|olglyph|olcursor|lucida'
```

If `mkfontdir` is missing, install your distro's X font utilities package.

## Runtime resources and defaults

The runtime package expects these assets to exist under `OPENWINHOME`:

- `lib/openwin-menu*`
- `lib/app-defaults/Olwm` and `lib/.text_extras_menu`
- `lib/help/filemgr.info`
- `share/locale/C/props`
- `share/terminfo`
- `share/fonts/openlook/bdf/{misc,75dpi,100dpi}`
- `include/images`
- `include/bitmaps` and `include/X11/bitmaps`

For staged local runs (`OPENWINHOME=<repo>/bin`), these are already in place
after build and are included by CMake install.

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
nested Xephyr session on `:2`. Native debugging requires GDB, Xephyr, and
X11 utilities (`xdpyinfo`, `xrdb`, `xset`, and `xprop`) on the host, alongside
the VS Code C/C++ extension. Use the Docker profile described above when
those tools are provided by the container. Launch applications from its Programs menu.
The Programs menu launches File Manager, MosaiX, Command Tool, Clock, Text Editor,
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
Xephyr :2 -resizeable -ac -screen 1024x768 &
export DISPLAY=:2
# Wait until xdpyinfo succeeds on :2, then configure that X server.
source ./bin/use-openwinhome.sh
olwm
```

Xephyr's window can be resized by dragging its border. olwm and XView
applications follow RandR changes to the X screen's virtual framebuffer:
placement bounds, Full Size windows, icon layout, and popup bounds update
without restarting. Mouse positions remain in the server's root coordinates.
Existing windows move back into view when the desktop shrinks. RandR outputs
share this desktop; individual monitor work areas are not implemented.
Building requires the RandR development library (`libxrandr-dev`).

Run the isolated resize and mouse-drag regression test after building:

```bash
bash tests/run-resize-smoke.sh
```

## Calculator

**Programs → Calculator** opens the native OPEN LOOK port of X.Org xcalc.
The source is in `apps/xcalc`, including its original calculation engine,
license, and [upstream provenance](apps/xcalc/UPSTREAM.txt). It builds locally
with CMake and XView, without downloading sources or requiring Xt/Athena.

The calculator has custom rounded pastel keys, a recessed display, algebraic
and RPN modes, trigonometry, logarithms, powers, factorials, parentheses, memory
and programmer functions. Use **F2**, **Ctrl+R**, or the right-click menu to
change modes; switching resets the calculation and memory.
Type arithmetic directly, use Enter to evaluate (or push in RPN), Backspace
to edit and Escape to clear. Results support native text selection and copying.
The supplied `calculator.png` is packaged through the shared icon pipeline.

Run `bin/bin/xcalc` (or `xcalc -rpn`) in the OpenLook environment. See `man xcalc`
for scientific shortcuts and memory-register behavior. The **base** key cycles
DEC/HEX/OCT. Programmer operations use checked unsigned 32-bit words; invalid
operands and shift counts show Error. Tab/arrows select a key and Space presses
it. Xt action resources are not used.

After building, `bash tests/run-calculator-smoke.sh` checks the arithmetic
engine and real XView controls in an isolated Xephyr desktop, including keyboard
and mouse input, RPN, error recovery, selected results, the icon and normal close.

## Virtual desktops

**Programs → Virtual Desktops** opens the native overview and enables a 3×2
grid in the existing `olwm`. This is an optional extension of our window
manager; it does not install or switch to `olvwm`. Ordinary sessions start
with one desktop unless configured otherwise.

- Click anywhere in a desktop, including a miniature, to switch; **Ctrl+Alt+arrow** moves
  to an adjacent desktop.
- Drag a miniature window into another cell to move it and its dependent
  dialogs. Double-click a miniature to restore and activate that window.
- Middle-click a miniature to pin it on every desktop; middle-click again
  to put it on the current desktop.
- Resize the pager to add or remove rows and columns. It snaps to fixed
  **80×72-pixel panes**, with 1–4 panes per axis. Shrink it to **1×1** to bring
  all windows back to a single desktop, preserving their minimized state.
  Existing panes keep their grid positions; windows from removed panes move
  to the last remaining pane. There is no Properties dialog.

The compact overview has dashed desktop separators and no toolbar or footer.
The current desktop’s separators become solid; the canvas has no inset border.
It uses `assets/icons/bw-theme/vwm.png`. Closing it leaves the
desktops active; opening it again raises the existing overview if present.
The grid, current desktop and assignments survive **Restart WM** on the same
X server. Exiting the WM restores access to all windows. New X sessions do
not retain assignments. To start a fresh session with six desktops, add
`OpenWindows.VirtualDesktop: 3x2` to your X resources.

Each desktop is screen-sized. Resizing Xephyr adjusts window bounds within
their assigned desktop; it does not expose hidden desktops. This version
provides a grid of separate desktops, rather than continuous panning across
one oversized workspace. The overview shows outlines and window names.

Builds and installs include `bin/vwm` and `man vwm`. Run the focused nested
Xephyr tests with `bash tests/run-virtual-desktop-smoke.sh`. Implementation
notes are in [docs/architecture/virtual-desktops.md](docs/architecture/virtual-desktops.md).

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
