#!/usr/bin/env python3
import argparse, json, os, sys
from PIL import Image, ImageDraw, ImageFont

'''
Zerlegen eines PNG images mit Boxen von "kraken segmentation"
'''

def run():
  parser = argparse.ArgumentParser(
    description='Split image using kraken segmentation')
  parser.add_argument('segments', metavar='seg',
    help='segment file (json)')
  parser.add_argument('image', metavar='img',
    help='Image')
  parser.add_argument('--dir', '-d', help='segment directory')
  nargs = parser.parse_args()
  args = vars(nargs)
  print(args)
  print(args['image'], args['segments'], args['dir'])

  im = Image.open(args['image']).convert("RGBA")
  draw = ImageDraw.Draw(im)
  try:
    fnt = ImageFont.truetype('DPCustomMono2.tff', 12)
  except:
    print('load default font')
    fnt = ImageFont.load_default()
  with open(args['segments'], 'r') as f:
    data = json.load(f)
  destDir = '.'
  if args['dir'] is not None:
    destDir = args['dir']
    os.makedirs(destDir, exist_ok=True)
  for i, b in enumerate(data['boxes']):
    filePath = os.path.join(destDir, 'l-{:03d}.png'.format(i))
    x0, y0, x1, y1 = b
    im1 = im.crop((x0, y0, x1, y1))
    im1.save(filePath)
  for i, b in enumerate(data['boxes']):
    x0, y0, x1, y1 = b
    draw.rectangle(((x0, y0), (x1, y1)), outline="red")
    draw.text((x0, y1-12), "l-{:02}".format(i), fill="black")
  im.save(os.path.join(destDir, 'segments.png'))

if __name__ == '__main__':
  run()
