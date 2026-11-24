#!/usr/bin/env python3
"""Exercise real XView scrollbar drags against coordinates reported by a page."""
import re
import subprocess
import sys
import time

window = sys.argv[1]


def xdo(*args):
    return subprocess.check_output(['xdotool', *map(str, args)], text=True).strip()


def position():
    title = xdo('getwindowname', window)
    match = re.search(r'MosaiX smoke ready (\d+),(\d+)', title)
    assert match, f'Unexpected browser title: {title}'
    return tuple(map(int, match.groups()))


def settled():
    time.sleep(0.6)
    value = position()
    for _ in range(5):
        time.sleep(0.15)
        assert position() == value, f'Scroll jumped after settling at {value}'
    return value


tree = subprocess.check_output(['xwininfo', '-id', window, '-children'], text=True)
children = []
for line in tree.splitlines():
    match = re.match(r'\s+(0x[0-9a-f]+).*?\s(\d+)x(\d+)\+(-?\d+)\+(-?\d+)', line)
    if match:
        xid, width, height, x, y = match.groups()
        children.append((xid, int(width), int(height), int(x), int(y)))
vertical = next(c for c in children if c[1] < 30 and c[2] > 200)
horizontal = next(c for c in children if c[2] < 30 and c[1] > 500 and c[4] > 300)
viewport = next(c for c in children if c[1] > 500 and c[2] > 200)


def drag(bar, axis, start, end, increasing=True):
    cross = (bar[1] if axis == 1 else bar[2]) // 2

    def move(along):
        x, y = (cross, along) if axis == 1 else (along, cross)
        xdo('mousemove', '--window', bar[0], x, y)

    before = position()
    move(start)
    xdo('mousedown', 1)
    values = []
    try:
        for step in range(1, 9):
            move(round(start + (end - start) * step / 8))
            time.sleep(0.16)
            values.append(position())
        held = settled()
    finally:
        xdo('mouseup', 1)
    after = settled()
    assert after == held, f'Releasing the thumb moved the page: {held} -> {after}'
    assert abs(after[axis] - before[axis]) > 400, (before, values, after)
    assert all(p[1 - axis] == before[1 - axis] for p in values), values
    offsets = [before[axis], *(p[axis] for p in values)]
    pairs = zip(offsets, offsets[1:])
    assert all(b >= a if increasing else b <= a for a, b in pairs), offsets
    return end


assert position() == (0, 0)
# The medium OpenLook elevator grip is 35 pixels from the top/start at offset 0.
drag(vertical, 1, 35, 300)
drag(horizontal, 0, 35, 480)
drag(vertical, 1, 300, 100, increasing=False)

# Page-wheel scrolling must update the native thumb without sending the old
# native position back to Chromium (including on a smooth-scrolling page).
xdo('mousemove', '--window', viewport[0], 300, 200, 'click', 1)
before = settled()
xdo('click', '--repeat', 4, '--delay', 120, 5)
after = settled()
assert after[1] > before[1] + 100 and after[0] == before[0], (before, after)

# A page-driven jump resets the thumb; dragging it again must start at zero.
xdo('key', 'ctrl+Home')
assert settled()[1] == 0
drag(vertical, 1, 35, 220)
print('Both scrollbar drags, held/released position, reverse drag, and wheel sync passed.')
