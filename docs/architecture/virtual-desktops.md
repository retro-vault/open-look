# Virtual desktops in olwm

The feature extends this repository's `olwm`; it does not import or launch
`olvwm`. The fork's virtual-desktop implementation was examined as a reference,
but its manager sources, parser and configuration language are not dependencies.

## Model and lifecycle

`src/olwm/virtual-desktop.c` owns separate per-screen and per-client records.
Existing `Client` and `ScreenInfo` layouts are unchanged. A record stores a
desktop index, or the all-desktops sentinel for a pinned window.

Each desktop has the physical screen's dimensions. Switching hides or maps
only the WM-owned frame or icon frame. It does not unmap the application's
pane or turn an ordinary window into an iconic/withdrawn window. Existing
ICCCM state transitions remain in `states.c`; their state-update hook reapplies
desktop visibility. The event hook also catches subsequent frame maps.

Transient and dependent group windows inherit their owner's assignment.
Moving or pinning a parent includes its dependent dialogs. Moving a dialog
moves its owner as well. Focus selection and keyboard cycling skip windows
on other desktops. The overview is pinned through the same desktop protocol.

The existing RandR handler still adjusts local window bounds and icon slots.
Because hidden desktops retain local coordinates, that adjustment cannot
accidentally bring their windows onto the current desktop. The desktop module
publishes the updated screen geometry after the bounds adjustment.

Removing a client drops its borrowed pointer before destruction. Shutdown
releases module records before normal olwm cleanup unparents and remaps client
panes. Grid/current-desktop root properties and client assignments survive a
WM restart on the same X server. A fresh X server starts at 1x1 unless the
`OpenWindows.VirtualDesktop` resource specifies another valid grid.

## Protocol and overview

`desktop-protocol.c` handles the desktop-specific subset of
[Extended Window Manager Hints](https://specifications.freedesktop.org/wm/latest/):
`_NET_NUMBER_OF_DESKTOPS`, `_NET_CURRENT_DESKTOP`, `_NET_WM_DESKTOP`,
`_NET_CLIENT_LIST`, desktop geometry, viewports, work areas and virtual roots.
Only the implemented properties are advertised in `_NET_SUPPORTED`.
Desktop indices are zero-based; `0xffffffff` means all desktops. Format-32
properties are masked to their wire width on 64-bit hosts.

Private properties/messages:

| Atom | Meaning |
| --- | --- |
| `_OPENLOOK_DESKTOP_VERSION` | Root CARDINAL value 1 while this WM module runs |
| `_OPENLOOK_DESKTOP_GRID` | Root CARDINAL pair: columns, rows; matching client message requests a grid |
| `_OPENLOOK_DESKTOP_ACTIVATE` | Client message targeting a managed window: switch, restore, raise and focus |

The grid is limited to four columns and four rows. Invalid requests are
ignored. Existing panes keep their row and column when the grid changes.
Reducing its size moves windows on removed desktops to the last remaining
desktop. A 1x1 grid releases the Ctrl+Alt+arrow grabs.

`apps/vwm` is a native XView pager using the shared icon loader. It owns a
separate X connection for queries so disappearing-window races can be handled
without suppressing XView errors. It refreshes its snapshot four times per
second, draws into a back buffer and sends requests to the WM. It never
directly manipulates application frames. A per-screen selection ensures that
launching it again raises the existing overview.

The compact canvas fills the window with dashed desktop separators, without
a toolbar, headings or footer. Middle-click pins or unpins a miniature.
A single click anywhere in a cell switches desktops, including over miniatures.
`grid.c` advertises 80x72 resize increments and one-to-four-pane bounds. Canvas
resizes request matching grids; frame snapping is deferred outside the toolkit
layout callback, and pending requests are reconciled with WM acknowledgements.
Pane sizes stay fixed. Closing the overview does not disable desktops;
resizing it to one pane does. There is no Properties dialog.
The program uses the supplied `vwm.png` through the existing PNG-to-XPM asset pipeline. Its manual is staged and installed
as `vwm(1)`.

## Validation and limits

Run `bash tests/run-virtual-desktop-smoke.sh` after building. Its isolated
Xephyr session covers switching, geometry, sticky groups, dependent dialogs,
minimize/restore, invalid messages, RandR resize, WM shutdown/restart, grid
reduction, withdrawal/remapping, overview mouse actions, shortcuts, icon
restoration, single-instance behavior and normal overview shutdown.
`tests/run-resize-smoke.sh` additionally covers existing single-desktop resize
and mouse-drag behavior.

This implementation uses discrete screen-sized desktops. Continuous panning,
windows spanning desktop cells, live screenshot thumbnails, and persistence
across new X sessions are outside its current scope. Keyboard shortcuts use
Ctrl+Alt plus the arrow keys while the feature is enabled.
