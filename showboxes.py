import json, sys
from PIL import Image, ImageDraw

'''
Zerlegen eines PNG images mit Boxen von "kraken segmentation"
'''

im = Image.open(sys.argv[1]).convert("RGBA")
draw = ImageDraw.Draw(im)
with open(sys.argv[2], 'r') as f:
  data = json.load(f)
for i, b in enumerate(data['boxes']):
  x0, y0, x1, y1 = b
  im1 = im.crop((x0, y0, x1, y1))
  im1.save('box-{:03d}.png'.format(i))
for i, b in enumerate(data['boxes']):
  x0, y0, x1, y1 = b
  draw.rectangle(((x0, y0), (x1, y1)), outline="red")
im.save('xxx.png')
