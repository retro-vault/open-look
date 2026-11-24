#!/usr/bin/env python3
"""Check the compact toolbar and dialog placement in the isolated smoke session."""
import re
import subprocess
import sys
import time

window, close_helper = sys.argv[1:]


def xdo(*args):
    return subprocess.check_output(['xdotool', *map(str, args)], text=True).strip()


def geometry(xid):
    text = subprocess.check_output(['xwininfo', '-id', str(xid)], text=True)
    return tuple(int(re.search(pattern, text).group(1)) for pattern in (
        r'Absolute upper-left X:\s+(-?\d+)', r'Absolute upper-left Y:\s+(-?\d+)',
        r'Width:\s+(\d+)', r'Height:\s+(\d+)'))


def children():
    text = subprocess.check_output(['xwininfo', '-id', window, '-children'], text=True)
    result = []
    for line in text.splitlines():
        match = re.match(r'\s+(0x[0-9a-f]+).*?\s(\d+)x(\d+)\+(-?\d+)\+(-?\d+)', line)
        if match:
            xid, width, height, x, y = match.groups()
            result.append((xid, int(width), int(height), int(x), int(y)))
    return result


def shown(name):
    for _ in range(60):
        result = subprocess.run(['xdotool', 'search', '--onlyvisible', '--name', name],
                                capture_output=True, text=True)
        if result.returncode == 0:
            return result.stdout.splitlines()[-1]
        time.sleep(0.1)
    raise AssertionError(f'Dialog did not appear: {name}')


def assert_centered(dialog):
    time.sleep(0.15)
    tree = subprocess.check_output(['xwininfo', '-id', dialog, '-tree'], text=True)
    parent = re.search(r'Parent window id: (0x[0-9a-f]+)', tree).group(1)
    dx, dy, dw, dh = geometry(parent)
    x, y, width, _ = geometry(window)
    footer = next(c for c in children() if c[1] == width and c[4] > 300)
    expected = (x + width / 2, y + footer[4] / 2)
    actual = (dx + dw / 2, dy + dh / 2)
    assert all(abs(a - b) <= 6 for a, b in zip(actual, expected)), (actual, expected)


def close_dialog(dialog):
    subprocess.run([close_helper, dialog], check=True)
    time.sleep(0.15)


def properties():
    width = geometry(window)[2]
    xdo('mousemove', '--window', window, width - 12, 12, 'click', 1)
    dialog = shown('^MosaiX: Properties$')
    assert_centered(dialog)
    return dialog


def assert_toolbar():
    width = geometry(window)[2]
    top = [c for c in children() if c[1] == width and c[4] < 100]
    assert len(top) == 1 and top[0][2:] == (26, 0, 0), top


# XView must advertise a real icon window, preserving display-depth artwork.
hints = subprocess.check_output(['xprop', '-id', window, 'WM_HINTS'], text=True)
icon = re.search(r'window id # to use for icon: (0x[0-9a-f]+)', hints).group(1)
assert geometry(icon)[2:] == (64, 64)
xdo('windowminimize', window)
time.sleep(0.3)
info = subprocess.check_output(['xwininfo', '-id', icon], text=True)
assert 'IsViewable' in info and int(re.search(r'Depth:\s+(\d+)', info).group(1)) > 1
xdo('mousemove', '--window', icon, 32, 32, 'click', '--repeat', 2, '--delay', 150, 1)
time.sleep(0.3)
assert 'IsViewable' in subprocess.check_output(['xwininfo', '-id', window], text=True)

assert_toolbar()
# A single click must replace the existing URL, rather than insert into it.
xdo('mousemove', '--window', window, 220, 10, 'click', 1)
xdo('type', '--clearmodifiers', '--delay', 50, 'about:blank')
xdo('key', 'Return')
assert shown('^(about:blank - )?MosaiX$') == window
xdo('mousemove', '--window', window, 88, 10, 'click', 1)
assert shown('^MosaiX smoke ready.* - MosaiX$') == window
time.sleep(0.2)
dialog = properties()
xdo('mousemove', '--window', dialog, 260, 345, 'click', 1)  # Reset stays open.
assert shown('^MosaiX: Properties$') == dialog
xdo('mousemove', '--window', dialog, 350, 345, 'click', 1)  # Cancel dismisses.
time.sleep(0.15)
assert subprocess.run(['xdotool', 'search', '--onlyvisible', '--name',
                       '^MosaiX: Properties$'], capture_output=True).returncode != 0
# Reused dialogs must follow the browser instead of remembering old coordinates.
xdo('windowmove', window, 110, 40, 'windowsize', window, 900, 640)
time.sleep(0.4)
assert_toolbar()
dialog = properties()
assert geometry(dialog)[2:] == (540, 374), geometry(dialog)
xdo('mousemove', '--window', dialog, 320, 263, 'click', 1)
notice = shown('^Notice$')
assert_centered(notice)
_, _, width, height = geometry(notice)
xdo('mousemove', '--window', notice, round(width * 0.7), height - 23, 'click', 1)
time.sleep(0.2)
assert shown('^MosaiX: Properties$') == dialog  # Data actions keep edits open.
close_dialog(dialog)

# Invalid input uses the same centered, application-owned notice path.
xdo('mousemove', '--window', window, 300, 200, 'click', 1, 'key', 'ctrl+l')
xdo('type', '--clearmodifiers', 'not a valid address')
xdo('key', 'Return')
notice = shown('^Notice$')
assert_centered(notice)
_, _, width, height = geometry(notice)
xdo('mousemove', '--window', notice, width // 2, height - 23, 'click', 1)
time.sleep(0.2)

# Alert, confirmation, and prompt are real renderer requests from the fixture.
viewport = next(c for c in children() if c[1] > 500 and c[2] > 200)
for x in (50, 105, 170):
    xdo('mousemove', '--window', viewport[0], x, 160, 'click', 1)
    dialog = shown('^MosaiX: Website Message$')
    assert_centered(dialog)
    close_dialog(dialog)
print('Application icon, minimize/restore, URL click replacement, toolbar, Properties, and dialog centering passed.')
