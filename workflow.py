#!/usr/bin/env python3
import argparse, json, os, subprocess, sys
from PIL import Image, ImageDraw, ImageFont

def boxQuartiles(boxes, idx):
  vals = list()
  for b in boxes:
    vals.append(b[idx])
  return quartiles(vals)

def quartiles(vals):
  vals = sorted(vals)
  lenVals = len(vals)
  q1 = lenVals//4
  q2 = lenVals//2
  q3 = 3*lenVals//4
  return vals[0], vals[q1], vals[q2], vals[q3], vals[-1]

def colDiffs(boxes, idx):
  start = True
  diffs = list()
  for b in boxes:
    if start:
      diffs.append(0)
      start = False
    else:
      diffs.append(b[idx]-oldVal)
    oldVal = b[idx]
  print('colDiffs', diffs)
  return diffs

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
    if fileName[-3:] != 'tif': continue
    i += 1
    print(i, fileName)
    pgNr = '{:03d}'.format(i)
    imgFile = os.path.join(imgDir, fileName)
    imgFiles.append((pgNr, imgFile,))
  return imgFiles

def splitImageFile(workDir, pgNr, imgFile):
  pageDir = os.path.join(workDir, pgNr)
  os.makedirs(pageDir, exist_ok=True)
  linesFile = os.path.join(pageDir, 'lines.json')
  subprocess.run(['kraken', '-i', imgFile, linesFile, 'segment'])
  return splitImage(imgFile, linesFile, pageDir)

def getModels(modelDir):
  models = list()
  for fileName in os.listdir(modelDir):
    parts = fileName.split('.')
    if parts[1] == 'ckpt':
      models.append(os.path.join(modelDir, parts[0]+'.ckpt'))
  return models

def ocr(lineFiles, models):
  cmdLine = ['calamari-predict', '--checkpoint'] + models + \
    ['--files'] + lineFiles
  print(cmdLine)
  subprocess.run(cmdLine)
 
def catLines(workDir, pgNr):
  inDir = os.path.join(workDir, pgNr)
  lines = list()
  for fileName in sorted(os.listdir(inDir)):
    parts = fileName.split('.')
    if parts[-1] != 'txt': continue
    fi = open(os.path.join(inDir, fileName))
    line = fi.readline()
    lines.append(line)
    fi.close()
  # TODO paragraph detection
  linesFile = os.path.join(workDir, pgNr, 'lines.json')
  fi = open(linesFile)
  data = json.load(fi)
  fi.close()
  boxes = data['boxes']
  qX0 = boxQuartiles(boxes, 0)
  qX1 = boxQuartiles(boxes, 2)
  lineDiffs = colDiffs(boxes, 3)
  qL = quartiles(lineDiffs)
  print('quartiles x0', qX0)
  print('quartiles diffs', qL)
  return '\n'.join(lines)

def writeText(outDir, pgNr, text):
  if text.strip() == '':
    text = '[Blank Page]'
  fileName = os.path.join(outDir, pgNr+'.txt')
  fo = open(fileName, 'w')
  fo.write(text)
  fo.write('\n')
  fo.close()

def run():
  parser = argparse.ArgumentParser(
    description='OCR workflow kraken/calamari')
  parser.add_argument('--imgdir', '-i',
    help='Images\' directory')
  parser.add_argument('--workdir', '-w',
    help='Images\' directory')
  parser.add_argument('--modeldir', '-m',
    help='Calamari models\' directory')
  parser.add_argument('--outdir', '-o',
    help='OCR output directory')
  nargs = parser.parse_args()
  args = vars(nargs)
  if args['workdir'] is None:
    print('Workdir missing')
    os.exit(1)
  os.makedirs(args['workdir'], exist_ok=True)
  os.makedirs(args['outdir'], exist_ok=True)
  models = getModels(args['modeldir'])
  for pgNr, imgFile in findImages(args['imgdir'], args['workdir']):
    lineFiles = splitImageFile(args['workdir'], pgNr, imgFile)
    ocr(lineFiles, models)
    txt = catLines(args['workdir'], pgNr)
    writeText(args['outdir'], pgNr, txt)

if __name__ == '__main__':
  run()
