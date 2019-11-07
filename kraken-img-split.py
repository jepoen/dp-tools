import argparse, json, os, sys
from PIL import Image, ImageDraw

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
  with open(args['segments'], 'r') as f:
    data = json.load(f)
  destDir = '.'
  if args['dir'] is not None:
    destDir = args['dir']
    os.makedirs(destDir, exist_ok=True)
  for i, b in enumerate(data['boxes']):
    filePath = os.path.join(destDir, 'line-{:03d}.png'.format(i))
    x0, y0, x1, y1 = b
    im1 = im.crop((x0, y0, x1, y1))
    im1.save(filePath)
  for i, b in enumerate(data['boxes']):
    x0, y0, x1, y1 = b
    draw.rectangle(((x0, y0), (x1, y1)), outline="red")
  im.save(os.path.join(destDir, 'segments.png'))

if __name__ == '__main__':
  run()
