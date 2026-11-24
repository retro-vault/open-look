#!/usr/bin/env python3
"""Size Tango artwork for the toolbar; needed only when changing icon assets."""
from pathlib import Path
from PIL import Image

icons = Path(__file__).resolve().parents[1] / 'apps/mosaix/icons'
names = ['go-previous', 'go-next', 'view-refresh', 'process-stop', 'go-home']
size = 13
lines = ['/* Tango 0.8.90 artwork resized to 13x13. See icons/UPSTREAM.txt.',
         ' * Generated from the adjacent 16x16 PNGs; alpha is preserved. */',
         f'static const unsigned char toolbar_rgba[5][{size} * {size} * 4] = {{']
for name in names:
    image = Image.open(icons / (name + '.png')).convert('RGBA')
    assert image.size == (16, 16)
    # Filter premultiplied colour so transparent pixels cannot add dark fringes.
    image = image.convert('RGBa').resize((size, size), Image.Resampling.LANCZOS).convert('RGBA')
    data = image.tobytes()
    lines.append('    { /* ' + name + ' */')
    for offset in range(0, len(data), 32):
        lines.append('        ' + ','.join(str(c) for c in data[offset:offset + 32]) + ',')
    lines.append('    },')
lines.append('};')
(icons / 'toolbar-rgba.h').write_text('\n'.join(lines) + '\n')
