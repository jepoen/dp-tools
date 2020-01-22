#!/usr/bin/env python3

import argparse, json, os, sys
from PIL import Image, ImageDraw

'''
Verbinden der Textzeilen mit kraken segment file
'''

def run():
  parser = argparse.ArgumentParser(
    description='Join predicted text using kraken segmentation')
  parser.add_argument('segments', metavar='seg',
    help='segment file (json)')
  parser.add_argument('text', metavar='txt',
    help='text file')
  parser.add_argument('--dir', '-d', help='segment directory')
  parser.add_argument('--pred', '-p', help='prediction file suffix (pred.txt)',
    default='pred.txt')
  nargs = parser.parse_args()
  args = vars(nargs)
  print(args['text'], args['segments'], args['dir'])

  with open(args['segments'], 'r') as f:
    data = json.load(f)
  destDir = '.'
  if args['dir'] is not None:
    destDir = args['dir']
    os.makedirs(destDir, exist_ok=True)
  lines = list()
  for i, b in enumerate(data['boxes']):
    filePath = os.path.join(destDir, 'l-{:03d}.{:s}'.format(i, args['pred']))
    x0, y0, x1, y1 = b
    with open(filePath) as fi:
      line = fi.read()
      lines.append(line)
    # TODO: Handle paragraphs
    text = '\n'.join(lines)
    with open(args['text'], 'w') as fo:
      fo.write(text)
      fo.write('\n')


if __name__ == '__main__':
  run()
