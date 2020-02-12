#!/usr/bin/env python3
import argparse, json, os, subprocess, sys
from PIL import Image, ImageDraw, ImageFont

"""
Split images in lineboxes

* tessseract imagefile workdir/pgfile/lstm.lstmbox lstmbox
* convert lstmbox to segment.json
* Split image into line files using segment.json

"""
# tesseract compiled for training
tesseractCmd = '/opt/tesseract/bin/tesseract'
# tesseract language model
tesseractLang = 'deu_frak'

def convertLstm2Lines(imgFile, lstmFile, pageDir):
  linesFile = os.path.join(pageDir, 'lines.json')
  im = Image.open(imgFile)
  w, h = im.size
  boxes = list()
  lineNr = 0
  txt = ''
  for line in open(lstmFile):
    if line[0] != '\t':
      if line[0] == ' ':
        txt += ' '
      else:
        txt += line.split(' ')[0]
    else:
      _, x0, y0, x1, y1, _ = line.split(' ')
      boxes.append((int(x0), h - int(y1), int(x1), h - int(y0),))
      txtFile = os.path.join(pageDir, 't-{:03d}.txt'.format(lineNr))
      with open(txtFile, 'w') as fo:
        fo.write(txt)
      lineNr += 1
      txt = ''
  fo = open(linesFile, 'w')
  json.dump({'boxes': boxes}, fo)
  fo.close()

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
  for pgNr, imgFile in pgImgFiles:
    pageDir = os.path.join(workDir, pgNr)
    pageFile = os.path.join(pageDir, 'page.png')
    lstmBaseFile = os.path.join(pageDir, pgNr)
    lstmFile = lstmBaseFile + '.box'
    os.makedirs(pageDir, exist_ok=True)
    print('Segmentation {} → {}'.format(imgFile, pageFile))
    subprocess.run(['convert', '-scale', '50%', imgFile, pageFile])
    subprocess.run([tesseractCmd, pageFile, lstmBaseFile, '-l', tesseractLang,
      'lstmbox'])
    convertLstm2Lines(pageFile, lstmFile, pageDir)
  for pgNr, _ in pgImgFiles:
    pageDir = os.path.join(workDir, pgNr)
    pageFile = os.path.join(pageDir, pgNr+'.png')
    linesFile = os.path.join(pageDir, 'lines.json')
    splitImage(pageFile, linesFile, pageDir)

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
