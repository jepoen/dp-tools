#!/usr/bin/env python3
import argparse, json, os, subprocess, sys
from PIL import Image, ImageDraw, ImageFont

"""
Split images in lineboxes

* kraken imagefile workdir/pgfile/segment.json segment
* Split image into line files using segment.json

"""

def splitImage(imgFile, linesFile, pageDir):
  im = Image.open(imgFile).convert("RGBA")
  draw = ImageDraw.Draw(im)
  try:
    fnt = ImageFont.truetype('DPCustomMono2.tff', 12)
  except:
    print('load default font')
    fnt = ImageFont.load_default()
  with open(linesFile, 'r') as f:
    data = json.load(f)
  lineFiles = list()
  for i, b in enumerate(data['boxes']):
    filePath = os.path.join(pageDir, 'l-{:03d}.png'.format(i))
    x0, y0, x1, y1 = b
    im1 = im.crop((x0, y0, x1, y1))
    im1.save(filePath)
    lineFiles.append(filePath)
  for i, b in enumerate(data['boxes']):
    x0, y0, x1, y1 = b
    draw.rectangle(((x0, y0), (x1, y1)), outline="red")
    draw.text((x0, y1-12), "l-{:02}".format(i), fill="black")
  im.save(os.path.join(pageDir, 'segments.png'))
  return lineFiles

def findImages(imgDir, workDir):
  i = 0
  imgFiles = list()
  for fileName in sorted(os.listdir(imgDir)):
    if fileName[-3:] not in ['png', 'tif']: continue
    i += 1
    print(i, fileName)
    pgNr = '{:03d}'.format(i)
    imgFile = os.path.join(imgDir, fileName)
    imgFiles.append((pgNr, imgFile,))
  return imgFiles

def splitImageFiles(workDir, pgImgFiles):
  cmdLine = ['kraken']
  for pgNr, imgFile in pgImgFiles:
    pageDir = os.path.join(workDir, pgNr)
    os.makedirs(pageDir, exist_ok=True)
    linesFile = os.path.join(pageDir, 'lines.json')
    cmdLine += ['-i', imgFile, linesFile]
  cmdLine.append('segment')
  print(cmdLine)
  subprocess.run(cmdLine)
  for pgNr, imgFile in pgImgFiles:
    pageDir = os.path.join(workDir, pgNr)
    linesFile = os.path.join(pageDir, 'lines.json')
    splitImage(imgFile, linesFile, pageDir)

def run():
  parser = argparse.ArgumentParser(
    description='OCR workflow kraken/calamari')
  parser.add_argument('--imgdir', '-i',
    help='Images\' directory')
  parser.add_argument('--workdir', '-w',
    help='Images\' directory')
  nargs = parser.parse_args()
  args = vars(nargs)
  if args['workdir'] is None:
    print('Workdir missing')
    os.exit(1)
  os.makedirs(args['workdir'], exist_ok=True)
  imgFiles = findImages(args['imgdir'], args['workdir'])
  lineFiles = splitImageFiles(args['workdir'], imgFiles)

if __name__ == '__main__':
  run()
