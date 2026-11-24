#!/usr/bin/env python3
"""Check native calculator input by reading its actual selected display text."""
import os
import re
import ctypes
import subprocess
import sys
import time

log_dir = sys.argv[1]


def run(*args):
    return subprocess.check_output(list(map(str, args)), text=True).strip()


def xdo(*args):
    return run('xdotool', *args)


def wait_for_state(target, wm_state, viewable, timeout=4):
    deadline = time.monotonic() + timeout
    while True:
        state = run('xprop', '-id', window, 'WM_STATE')
        info = run('xwininfo', '-id', target)
        if (f'window state: {wm_state}' in state and
                ('Map State: IsViewable' in info) == viewable):
            return info
        if time.monotonic() >= deadline:
            raise AssertionError(
                f'Timed out waiting for {target}: WM_STATE={wm_state}, '
                f'viewable={viewable}\n{state}\n{info}')
        time.sleep(0.05)


def uncover_icon(icon):
    # xdotool cannot lower windows, and its pointer query excludes WM icons.
    # Use Xlib to lower normal clients and verify the actual root child hit.
    xlib = ctypes.CDLL('libX11.so.6')
    display_type = ctypes.c_void_p
    xid_type = ctypes.c_ulong
    xid_pointer = ctypes.POINTER(xid_type)
    int_pointer = ctypes.POINTER(ctypes.c_int)
    xlib.XOpenDisplay.argtypes = [ctypes.c_char_p]
    xlib.XOpenDisplay.restype = display_type
    xlib.XLowerWindow.argtypes = [display_type, xid_type]
    xlib.XFlush.argtypes = [display_type]
    xlib.XCloseDisplay.argtypes = [display_type]
    xlib.XQueryPointer.argtypes = [display_type, xid_type, xid_pointer, xid_pointer,
                                  int_pointer, int_pointer, int_pointer, int_pointer,
                                  ctypes.POINTER(ctypes.c_uint)]
    display = xlib.XOpenDisplay(None)
    assert display, 'Cannot connect to the test display'
    try:
        tree = run('xwininfo', '-id', icon, '-tree')
        root = int(re.search(r'Root window id: (0x[0-9a-f]+)', tree).group(1), 16)
        frame = int(re.search(r'Parent window id: (0x[0-9a-f]+)', tree).group(1), 16)
        clients = run('xprop', '-root', '_NET_CLIENT_LIST')
        for client in re.findall(r'0x[0-9a-f]+', clients):
            if int(client, 16) != int(window, 0):
                xlib.XLowerWindow(display, int(client, 16))
        xlib.XFlush(display)
        xdo('mousemove', '--window', icon, 32, 24)
        root_return, child = xid_type(), xid_type()
        root_x, root_y, x, y = (ctypes.c_int() for _ in range(4))
        mask = ctypes.c_uint()
        deadline = time.monotonic() + 4
        while True:
            same_screen = xlib.XQueryPointer(
                display, root, ctypes.byref(root_return), ctypes.byref(child),
                ctypes.byref(root_x), ctypes.byref(root_y), ctypes.byref(x),
                ctypes.byref(y), ctypes.byref(mask))
            if same_screen and child.value == frame:
                return
            if time.monotonic() >= deadline:
                raise AssertionError(f'Icon frame {frame:#x} is covered by {child.value:#x}')
            time.sleep(0.05)
    finally:
        xlib.XCloseDisplay(display)


window = xdo('search', '--name', '^Calculator$').splitlines()[-1]


def expect(value):
    time.sleep(0.1)
    xdo('mousemove', '--window', window, 196, 18, 'click', '--repeat', 2,
        '--delay', 80, 1)
    actual = run(os.path.join(log_dir, 'read-selection'))
    assert actual == value, (value, actual)


def key(button):
    layout = [0,1,2,3,4, 5,6,7,8,9, 10,11,12,13,14,
              50,47,51,54,55, 15,16,17,18,48, 52,59,60,61,53,
              49,56,57,58,19, 20,21,22,23,24, 25,26,27,28,29,
              30,31,32,33,34, 35,36,37,38,39]
    index = layout.index(button)
    xdo('mousemove', '--window', window, 27 + index % 5 * 42,
        80 + index // 5 * 28, 'click', 1)


xdo('windowfocus', window)
xdo('type', '--clearmodifiers', '123+456=')
expect('579')
key(4)  # AC
key(32)  # 2
key(34)  # +
key(33)  # 3
key(39)  # =
expect('5')
xdo('key', 'Escape')
xdo('type', '--clearmodifiers', '9')
key(1)  # square, then keep typing after a native button click
xdo('type', '--clearmodifiers', '+1=')
expect('82')
xdo('key', 'Escape')
xdo('type', '--clearmodifiers', '30s')
expect('0.5')
xdo('key', 'Escape')
xdo('type', '--clearmodifiers', '1/0=')
expect('Error')
key(3)  # CE/C recovers from error
xdo('type', '--clearmodifiers', '2+3=')
expect('5')

# RPN shortcut switches the actual UI, including Enter semantics.
xdo('key', 'F2')
xdo('type', '--clearmodifiers', '2')
xdo('key', 'Return')
xdo('type', '--clearmodifiers', '3+')
expect('5')
xdo('key', 'F2')
# Programmer keys use the displayed number base and checked word operations.
xdo('type', '--clearmodifiers', '255')
key(48)  # base: DEC -> HEX
expect('FF')
key(47)  # and
key(61)  # F
key(39)
expect('F')
key(48)  # OCT
expect('17')
key(48)  # DEC
expect('15')
xdo('key', 'Escape')
xdo('type', '--clearmodifiers', '5')
key(52)  # shl
key(32)  # 2
key(39)
expect('20')

# Pointer activation commits only on release in the original key.
xdo('key', 'Escape')
xdo('mousemove', '--window', window, 69, 276, 'mousedown', 1)
run('bash', '-c', 'DISPLAY=:99 xwd -root -silent | convert xwd:- ' +
    os.path.join(log_dir, 'pressed.png'))
xdo('mousemove', '--window', window, 2, 2, 'mouseup', 1)
expect('0')
xdo('mousemove', '--window', window, 69, 276, 'mousedown', 1,
    'mousemove', '--window', window, 2, 2, 'mouseup', 1)
expect('0')
key(32)  # select 2, then keyboard-focus the adjacent 3 and activate with Space
xdo('key', 'Right', 'space')
expect('23')
xdo('key', 'Escape')
xdo('type', '--clearmodifiers', '123+456=')
expect('579')
run('bash', '-c', 'DISPLAY=:99 xwd -root -silent | convert xwd:- ' +
    os.path.join(log_dir, 'calculator.png'))

# Minimized icon comes from the supplied calculator asset and restores normally.
hint = run('xprop', '-id', window, 'WM_HINTS')
icon = re.search(r'window id # to use for icon: (0x[0-9a-f]+)', hint).group(1)
xdo('windowminimize', window)
info = wait_for_state(icon, 'Iconic', True)
assert 'Width: 64' in info and 'Height: 64' in info
run('bash', '-c', 'DISPLAY=:99 xwd -root -silent | convert xwd:- ' +
    os.path.join(log_dir, 'icon.png'))
# IsViewable means mapped, not unobscured: ensure the icon receives the clicks.
uncover_icon(icon)
xdo('mousemove', '--window', icon, 32, 24, 'click', '--repeat', 2,
    '--delay', 80, 1)
wait_for_state(window, 'Normal', True)
print('Custom keys, drag cancellation, keyboard focus, scientific/programmer/RPN modes, selection and icon passed.')
