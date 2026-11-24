#!/usr/bin/env python3
"""Convert the supplied application artwork to XPM; Pillow is regeneration-only."""
from pathlib import Path
from PIL import Image

root = Path(__file__).resolve().parents[1]
source = root / 'assets/icons/MosaiX-64.png'
target = source.with_suffix('.xpm')
image = Image.open(source).convert('RGBA')
assert image.size == (64, 64), 'MosaiX application artwork must be 64 by 64'
pixels = [image.getpixel((x, y)) for y in range(64) for x in range(64)]
assert all(p[3] in (0, 255) for p in pixels), 'XPM requires binary transparency'
colours = sorted(set(None if p[3] == 0 else p[:3] for p in pixels),
                 key=lambda p: (-1, -1, -1) if p is None else p)
alphabet = 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789'
assert len(colours) <= len(alphabet) ** 2
cpp = 1 if len(colours) <= len(alphabet) else 2
keys = {colour: (alphabet[i] if cpp == 1 else
                 alphabet[i // len(alphabet)] + alphabet[i % len(alphabet)])
        for i, colour in enumerate(colours)}
lines = ['/* XPM */', '/* Generated from MosaiX-64.png by tools/convert-mosaix-icon.py. */',
         'static const char *mosaix_icon[] = {', f'"64 64 {len(colours)} {cpp}",']
for colour in colours:
    value = 'None' if colour is None else '#%02x%02x%02x' % colour
    lines.append(f'"{keys[colour]} c {value}",')
for y in range(64):
    row = ''.join(keys[None if p[3] == 0 else p[:3]] for p in pixels[y * 64:(y + 1) * 64])
    lines.append('"' + row + '",')
lines.append('};')
target.write_text('\n'.join(lines) + '\n')
