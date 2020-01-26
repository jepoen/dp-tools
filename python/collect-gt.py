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

def rmFiles(basedir, subdir):
  dirName = os.path.join(basedir, subdir)
  for f in os.listdir(dirName):
    os.remove(os.path.join(dirName, f))

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
  parser.add_argument('--clean', '-c', action='store_const',
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
  trainDir = os.path.join(args['outdir'], 'train')
  validDir = os.path.join(args['outdir'], 'valid')
  testDir = os.path.join(args['outdir'], 'test')
  os.makedirs(trainDir, exist_ok=True)
  os.makedirs(testDir, exist_ok=True)
  os.makedirs(validDir, exist_ok=True)
  if args['clean']:
    for dirName in ['train', 'test', 'valid']:
      rmFiles(args['outdir'], dirName)
  testSplit = float(args['testsplit'])
  validSplit = float(args['validsplit'])
  imgTxtList = collectFiles(args['workdir'])
  random.shuffle(imgTxtList)
  validIdx = int(len(imgTxtList)*validSplit)
  testIdx = int(len(imgTxtList)*(validSplit+testSplit))
  print('Validation:', validIdx)
  print('Test:      ', testIdx-validIdx)
  print('Training:', len(imgTxtList)-testIdx)
  copyFiles(imgTxtList[:validIdx], args['workdir'], validDir, args['prefix'])
  copyFiles(imgTxtList[validIdx:testIdx], args['workdir'], testDir, args['prefix'])
  copyFiles(imgTxtList[testIdx:], args['workdir'], trainDir, args['prefix'])

if __name__ == '__main__':
  run()
