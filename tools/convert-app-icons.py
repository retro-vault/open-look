#!/usr/bin/env python3
"""Convert the supplied 64px theme to lossless XPMs (Pillow: regeneration only)."""
from pathlib import Path
from PIL import Image

theme = Path(__file__).resolve().parents[1] / 'assets/icons/bw-theme'
output = theme / 'xpm'
output.mkdir(exist_ok=True)
alphabet = 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789'
for source in sorted(theme.glob('*.png')):
    image = Image.open(source).convert('RGBA')
    assert image.size == (64, 64), source
    pixels = [image.getpixel((x, y)) for y in range(64) for x in range(64)]
    assert all(p[3] in (0, 255) for p in pixels), f'{source}: binary alpha required'
    colours = sorted(set(None if not p[3] else p[:3] for p in pixels),
                     key=lambda p: (-1, -1, -1) if p is None else p)
    cpp = 1
    while len(alphabet) ** cpp < len(colours):
        cpp += 1
    keys = {colour: ''.join(alphabet[(i // len(alphabet) ** n) % len(alphabet)]
                           for n in reversed(range(cpp)))
            for i, colour in enumerate(colours)}
    lines = ['/* XPM */', f'/* Generated from {source.name}; tools/convert-app-icons.py. */',
             'static const char *app_icon[] = {', f'"64 64 {len(colours)} {cpp}",']
    for colour in colours:
        value = 'None' if colour is None else '#%02x%02x%02x' % colour
        lines.append(f'"{keys[colour]} c {value}",')
    for y in range(64):
        row = ''.join(keys[None if not p[3] else p[:3]] for p in pixels[y * 64:(y + 1) * 64])
        lines.append(f'"{row}",')
    lines.append('};')
    (output / (source.stem + '.xpm')).write_text('\n'.join(lines) + '\n')
