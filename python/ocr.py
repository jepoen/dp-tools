#!/usr/bin/env python3
import argparse, json, os, subprocess, sys
from PIL import Image, ImageDraw, ImageFont

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

def getLineFiles(workDir):
  linefiles = list()
  for pgNr in sorted(os.listdir(workDir)):
    pageDir = os.path.join(workDir, pgNr)
    for f in sorted(os.listdir(pageDir)):
      parts = f.split(".")
      if parts[-1] == 'png':
        linefiles.append(os.path.join(pageDir, f))
  return linefiles

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

def makeParas(lines, paragraphs):
  text = ""
  for l, p in zip(lines, paragraphs):
    if p:
      text += '\n'
    text += l + '\n'
  return text

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
  lineCount = len(boxes)
  paraList = [False]*lineCount
  if lineCount == 0: #empty page
    return ''
  if lineCount > 1:
    qX0 = boxQuartiles(boxes, 0)
    qX1 = boxQuartiles(boxes, 2)
    lineDiffs = colDiffs(boxes, 3)
    qLD = quartiles(lineDiffs[1:])
    lLens = lineLens(boxes, 0, 2)
    qLL = quartiles(lLens)
    print('quartiles x0', qX0)
    print('quartiles diffs', qLD)
    print('quartiles lens', qLL)
    for i, b in enumerate(boxes):
      x0, y0, x1, y1 = b
      if abs(x0 - qX0[2]) > 0.05*qLL[2]:
        paraList[i] = True
      elif abs(x1 -qX1[2]) > 0.05*qLL[2] and i < lineCount-1:
        paraList[i+1] = True
      elif i > 0 and lineDiffs[i] > 1.5*qLD[2]:
        paraList[i] = True
  paraFile = os.path.join(workDir, pgNr, 'paragraphs.json')
  with open(paraFile, 'w') as fo:
    json.dump({'paragraphs': paraList}, fo)
  return makeParas(lines, paraList)

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
  parser.add_argument('--workdir', '-w',
    help='Work\' directory')
  parser.add_argument('--modeldir', '-m',
    help='Calamari models\' directory')
  nargs = parser.parse_args()
  args = vars(nargs)
  if args['workdir'] is None:
    print('Workdir missing')
    return
    os.exit(1)
  models = getModels(args['modeldir'])
  lineFiles = getLineFiles(args['workdir'])
  print(lineFiles)
  ocr(lineFiles, models)

if __name__ == '__main__':
  run()
