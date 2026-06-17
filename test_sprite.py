import sys
from PIL import Image

img = Image.open('assets/zombies/1ZombieSpriteSheet.png')
img = img.convert('RGBA')

w, h = img.size
col_has_pixels = []
for x in range(w):
    has_px = False
    for y in range(h):
        r, g, b, a = img.getpixel((x, y))
        if a > 0:
            has_px = True
            break
    col_has_pixels.append(has_px)

cuts = []
for i in range(1, len(col_has_pixels)):
    if col_has_pixels[i-1] and not col_has_pixels[i]:
        cuts.append(i)

print(f"Width: {w}, Height: {h}")
print("Non-empty column ranges:")
start = None
for i in range(w):
    if col_has_pixels[i] and start is None:
        start = i
    elif not col_has_pixels[i] and start is not None:
        print(f"[{start} - {i-1}] (width: {i-start})")
        start = None
if start is not None:
    print(f"[{start} - {w-1}] (width: {w-start})")

