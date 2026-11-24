#!/usr/bin/env python3
"""Exercise the overview with real pointer/keyboard input in nested Xephyr."""
import os
import re
import subprocess
import time


def run(*args):
    return subprocess.check_output(list(map(str, args)), text=True).strip()


def xdo(*args):
    return run('xdotool', *args)


def prop(window, name):
    target = ['-root'] if window is None else ['-id', str(window)]
    text = run('xprop', *target, name)
    return int(text.rsplit('=', 1)[-1].strip().split(',')[0], 0)


def wait_for(predicate, message):
    for _ in range(60):
        if predicate():
            return
        time.sleep(0.05)
    raise AssertionError(message)


def geometry(window):
    text = run('xwininfo', '-id', window)
    return tuple(int(re.search(pattern, text).group(1)) for pattern in (
        r'Absolute upper-left X:\s+(-?\d+)', r'Absolute upper-left Y:\s+(-?\d+)',
        r'Width:\s+(\d+)', r'Height:\s+(\d+)'))


def parent(window):
    text = run('xwininfo', '-id', window, '-tree')
    return re.search(r'Parent window id: (0x[0-9a-f]+)', text).group(1)


def visible(window):
    return 'Map State: IsViewable' in run('xwininfo', '-id', window)


def find(name):
    return xdo('search', '--name', name).splitlines()[-1]


overview = find('^Virtual Desktops$')
clock = xdo('search', '--class', '^clock$').splitlines()[-1]
xdo('windowraise', overview)
wait_for(lambda: prop(None, '_NET_NUMBER_OF_DESKTOPS') == 6, 'default grid')
tree = run('xwininfo', '-id', overview, '-tree')
paint = None
for line in tree.splitlines():
    match = re.match(r'\s+(0x[0-9a-f]+).*?\s(\d+)x(\d+)\+', line)
    if match and int(match[2]) >= 120 and int(match[3]) >= 80:
        paint = match[1]  # deepest canvas paint window
assert paint
_, _, width, height = geometry(paint)
cell_width = width // 3
cell_height = height // 2
# The flat grid fills the client area without an inset canvas border.
ox, oy, ow, oh = geometry(overview)
px, py, _, _ = geometry(paint)
assert (px - ox, py - oy, ow - width, oh - height) == (0, 0, 0, 0)


def cell(index):
    return (index % 3) * width // 3, (index // 3) * height // 2


def click_desktop(index):
    x, y = cell(index)
    xdo('mousemove', '--window', paint, x + cell_width - 6, y + 5, 'click', 1)
    wait_for(lambda: prop(None, '_NET_CURRENT_DESKTOP') == index, 'desktop click')
    time.sleep(0.3)  # allow the overview's snapshot to reflect the WM update


def clock_point(index):
    x, y, w, h = geometry(parent(clock))
    root_text = run('xwininfo', '-root')
    sw = int(re.search(r'Width:\s+(\d+)', root_text).group(1))
    sh = int(re.search(r'Height:\s+(\d+)', root_text).group(1))
    cx, cy = cell(index)
    mx = cx + 2 + x * (cell_width - 4) // sw
    my = cy + 2 + y * (cell_height - 4) // sh
    mw = max(8, w * (cell_width - 4) // sw)
    mh = max(8, h * (cell_height - 4) // sh)
    return min(mx + mw // 2, cx + cell_width - 3), min(my + mh // 2, cy + cell_height - 3)


time.sleep(0.4)
start = clock_point(0)
target = cell(1)
xdo('mousemove', '--window', paint, *start, 'mousedown', 1)
xdo('mousemove', '--window', paint, target[0] + cell_width // 2, target[1] + cell_height // 2)
xdo('mouseup', 1)
wait_for(lambda: prop(clock, '_NET_WM_DESKTOP') == 1, 'drag window to desktop')
assert not visible(clock)
# A single click over a miniature must switch even when the pager lacks focus.
for _ in range(3):
    click_desktop(0)
    xdo('mousemove', '--window', paint, *clock_point(1), 'click', 1)
    wait_for(lambda: prop(None, '_NET_CURRENT_DESKTOP') == 1,
             'single click over a miniature switches desktop')
    assert visible(clock)
    time.sleep(0.3)
    xdo('windowfocus', clock)
time.sleep(0.3)
xdo('mousemove', '--window', paint, *clock_point(1), 'click', 2)
wait_for(lambda: prop(clock, '_NET_WM_DESKTOP') == 0xffffffff, 'pin window')
click_desktop(0)
assert visible(clock)
xdo('mousemove', '--window', paint, *clock_point(0), 'click', 2)
wait_for(lambda: prop(clock, '_NET_WM_DESKTOP') == 0, 'unpin window')
xdo('key', 'ctrl+alt+Right')
wait_for(lambda: prop(None, '_NET_CURRENT_DESKTOP') == 1, 'keyboard switching')
assert not visible(clock)
xdo('key', 'ctrl+alt+Left')
wait_for(lambda: prop(None, '_NET_CURRENT_DESKTOP') == 0, 'keyboard return')
xdo('key', 'Num_Lock', 'ctrl+alt+Right')
wait_for(lambda: prop(None, '_NET_CURRENT_DESKTOP') == 1, 'shortcut with Num Lock')
xdo('key', 'ctrl+alt+Left', 'Num_Lock')
wait_for(lambda: prop(None, '_NET_CURRENT_DESKTOP') == 0, 'return with Num Lock')

# The supplied icon is used and restoring/relaunching reuses the same overview.
hint = run('xprop', '-id', overview, 'WM_HINTS')
icon = re.search(r'window id # to use for icon: (0x[0-9a-f]+)', hint).group(1)
xdo('windowminimize', overview)
wait_for(lambda: visible(icon), 'overview icon')
assert geometry(icon)[2:] == (64, 64)
subprocess.run([os.path.join(os.environ['OPENWINHOME'], 'bin/vwm')], check=True,
               timeout=5)
wait_for(lambda: visible(overview), 'reopen existing overview')
assert len(xdo('search', '--name', '^Virtual Desktops$').splitlines()) == 1

# Keep a screenshot of the compact six-desktop layout before changing the grid.
subprocess.run(['bash', '-c',
    'xwd -root -silent | convert xwd:- build/docker/vwm-smoke/compact.png'],
    check=True)

# Right-click no longer creates a Properties dialog.
xdo('mousemove', '--window', paint, width - 6, height - 6, 'click', 3)
time.sleep(0.2)
assert subprocess.run(['xdotool', 'search', '--name',
                       '^Virtual Desktops: Properties$'],
                      stdout=subprocess.DEVNULL).returncode != 0


def expect_grid(columns, rows):
    wait_for(lambda: prop(None, '_NET_NUMBER_OF_DESKTOPS') == columns * rows,
             f'resize creates {columns}x{rows} grid')
    wait_for(lambda: geometry(overview)[2:] == (columns * 80, rows * 72),
             'pager snaps to whole fixed-size panes')
    wait_for(lambda: geometry(paint)[2:] == (columns * 80, rows * 72),
             'canvas follows the grid size')
    grid = run('xprop', '-root', '_OPENLOOK_DESKTOP_GRID').rsplit('=', 1)[-1]
    assert tuple(map(int, grid.split(','))) == (columns, rows)


# A real corner drag adds one column and row, preserving native size increments.
fx, fy, fw, fh = geometry(parent(overview))
# Send real input through Xephyr's host while olwm holds its resize grab.
run('env', 'DISPLAY=:99', 'xdotool',
    'mousemove', fx + fw - 2, fy + fh - 2, 'mousedown', 1,
    'sleep', 0.1, 'mousemove', fx + fw - 2 + 80, fy + fh - 2 + 72,
    'sleep', 0.1, 'mouseup', 1)
expect_grid(4, 3)

# Non-grid configure requests snap down: add panes only when they fit.
xdo('windowsize', overview, 319, 215)
expect_grid(3, 2)
xdo('windowsize', overview, 320, 288)
expect_grid(4, 4)
hints = run('xprop', '-id', overview, 'WM_NORMAL_HINTS')
assert re.search(r'resize increment: 80 by 72', hints), hints

# Shrinking to one pane reunites windows and leaves a usable fixed-size cell.
xdo('windowsize', overview, 80, 72)
expect_grid(1, 1)
assert visible(clock)
assert prop(clock, '_NET_WM_DESKTOP') == 0
print('Single-click switching, drag, pinning, shortcuts, reopen and resize grid passed.')
