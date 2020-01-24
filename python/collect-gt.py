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
  parser.add_argument('--outdir', '-o', default='.',
    help='output directory (<outdir>/train <outdir>/validate <outdir>/test)')
  parser.add_argument('--validsplit', '-v', default='0.2',
    help='Validation split (ratio)')
  parser.add_argument('--testsplit', '-t', default='0.2',
    help='Test split (ratio)')
  parser.add_argument('--clean', '-c', nargs='?',
    help='Clean output directory', const=True, default=False)
  parser.add_argument('--prefix', '-p',
    help='file prefix')
  nargs = parser.parse_args()
  args = vars(nargs)
  print(args)
  if args['workdir'] is None:
    print('Workdir missing')
    sys.exit(1)
  if args['prefix'] is None:
    now = datetime.datetime.now()
    args['prefix'] = now.strftime('%Y-%m-%dT%H:%M')
  os.makedirs(os.path.join(args['outdir'], 'train'), exist_ok=True)
  os.makedirs(os.path.join(args['outdir'], 'test'), exist_ok=True)
  os.makedirs(os.path.join(args['outdir'], 'valid'), exist_ok=True)
  os.makedirs(args['valdir'], exist_ok=True)
  imgTxtList = collectFiles(args['workdir'])
  random.shuffle(imgTxtList)
  splitIdx = len(imgTxtList)*args['valsize']//100
  print('Validation:', splitIdx, 'Training:', len(imgTxtList)-splitIdx)
  copyFiles(imgTxtList[splitIdx:], args['workdir'], args['traindir'], args['prefix'])
  copyFiles(imgTxtList[:splitIdx], args['workdir'], args['valdir'], args['prefix'])

if __name__ == '__main__':
  run()
