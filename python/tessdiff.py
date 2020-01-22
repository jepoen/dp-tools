#!/usr/bin/env python3
import argparse, os, re, sys

def cleanCalamari(txt):
  txt = txt.replace('ſ', 's')
  txt = txt.replace(',,', '»')
  txt = txt.replace('\'\'', '«')
  return txt

def diff(workDir, outfile):
  reNr = re.compile(r'^l-([0-9]{3}).pred.txt$')
  for dirName in sorted(os.listdir(workDir)):
    path = os.path.join(workDir, dirName)
    if not os.path.isdir(path): continue
    for fileName in sorted(os.listdir(path)):
      m = reNr.match(fileName)
      if m is None: continue
      tessFileName = os.path.join(path, 't-{}.txt'.format(m.group(1)))
      if not os.path.exists(tessFileName):
        print('Missing tesseract file', tessFileName)
        continue
      with open(os.path.join(path, fileName)) as fi:
        calamariText = fi.read()
        calamariText = cleanCalamari(calamariText)
      with open(tessFileName) as fi:
        tessText = fi.read()
      if tessText != calamariText:
        print(os.path.join(path, fileName))
        print('Tesseract:', tessText)
        print('Calamari :', calamariText)
      
def run():
  parser = argparse.ArgumentParser(
    description='Difference file between tesseract and calamari')
  parser.add_argument('--workdir', '-w',
    help='Images\' directory')
  parser.add_argument('--out', '-o',
    help='output file (HTML)')
  nargs = parser.parse_args()
  args = vars(nargs)
  if args['workdir'] is None:
    print('Workdir missing')
    os.exit(1)
  if args['out'] is None:
    args['out'] = 'diff.html'
  diff(args['workdir'], args['out'])
if __name__ == '__main__':
  run()

