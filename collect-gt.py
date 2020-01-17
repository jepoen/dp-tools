#!/usr/bin/env python3
import argparse, datetime, os, random, shutil, sys

def collectFiles(workdir):
  result = list()
  for pg in sorted(os.listdir(workdir)):
    pgDir = os.path.join(workdir, pg)
    for f in sorted(os.listdir(pgDir)):
      parts = f.split('.')
      if parts[-2] != 'gt' or parts[-1] != 'txt': continue
      gtFile = os.path.join(pg, f)
      imgFileName = '.'.join(parts[:-2]) + '.png'
      imgFile = os.path.join(pg, imgFileName)
      if not os.path.exists(os.path.join(workdir, imgFile)): continue
      result.append((gtFile, imgFile))
  return result

def copyFiles(fileList, workDir, tgtDir, prefix):
  for gtFile, imgFile in fileList:
    newGtFile = prefix+'-'+gtFile.replace('/', '-')
    newImgFile = prefix+'-'+imgFile.replace('/', '-')
    shutil.copyfile(os.path.join(workDir, gtFile), os.path.join(tgtDir, newGtFile))
    shutil.copyfile(os.path.join(workDir, imgFile), os.path.join(tgtDir, newImgFile))

def run():
  parser = argparse.ArgumentParser(
    description='Collect ground truth files')
  parser.add_argument('--workdir', '-w',
    help='Work\' directory')
  parser.add_argument('--traindir', '-t',
    help='Training output directory')
  parser.add_argument('--valdir', '-v',
    help='Validation output directory')
  parser.add_argument('--prefix', '-p',
    help='file prefix')
  parser.add_argument('--valsize', '-s',
    help='size of validation set (%)')
  nargs = parser.parse_args()
  args = vars(nargs)
  if args['workdir'] is None:
    print('Workdir missing')
    sys.exit(1)
  if args['traindir'] is None:
    print('Traindir missing')
    sys.exit(1)
  if args['valdir'] is None:
    print('Validation dir missing')
    sys.exit(1)
  if args['prefix'] is None:
    now = datetime.datetime.now()
    args['prefix'] = now.strftime('%Y-%m-%dT%H:%M')
  if args['valsize'] is None:
    args['valsize'] = 20
  os.makedirs(args['traindir'], exist_ok=True)
  os.makedirs(args['valdir'], exist_ok=True)
  imgTxtList = collectFiles(args['workdir'])
  random.shuffle(imgTxtList)
  splitIdx = len(imgTxtList)*args['valsize']//100
  print('Validation:', splitIdx, 'Training:', len(imgTxtList)-splitIdx)
  copyFiles(imgTxtList[splitIdx:], args['workdir'], args['traindir'], args['prefix'])
  copyFiles(imgTxtList[:splitIdx], args['workdir'], args['valdir'], args['prefix'])

if __name__ == '__main__':
  run()
