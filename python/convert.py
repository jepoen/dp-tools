import argparse, os, subprocess

def convert(imgDir, workDir, lang, scale):
  i = 0
  fo = open('pretess.sh', 'w')
  os.makedirs(workDir, exist_ok=True)
  for f in sorted(os.listdir(imgDir)):
    parts = f.split('.')
    if parts[-1] != 'tif': continue
    i += 1
    tgtBase = '{}/{:03d}'.format(workDir, i)
    tgt = tgtBase + '.png'
    src = os.path.join(imgDir, f)
    cmd = ['convert', '-depth', '1', '-scale', '{}%'.format(scale), src, tgt]
    print(' '.join(cmd))
    fo.write(' '.join(cmd))
    fo.write('\n')
    subprocess.call(cmd)
    cmd = ['tesseract', tgt, tgtBase, '--psm', '4', '-l', lang, 'boxtext']
    print(' '.join(cmd))
    fo.write(' '.join(cmd))
    fo.write('\n')
    subprocess.call(cmd)
    cmd = ['tesslineboxes', '-basename', tgtBase,
      '-image', tgt]
    print(' '.join(cmd))
    fo.write(' '.join(cmd))
    fo.write('\n')
    subprocess.call(cmd)
  fo.close()

def run():
  parser = argparse.ArgumentParser(
    description='Convert Scantailor output to png, preprocess with tesseract'
  )
  parser.add_argument('--workdir', '-w', default='tmp',
    help='Work dir (output)')
  parser.add_argument('--imgdir', '-i', default='scan/out',
    help='Tesseract Image dir')
  parser.add_argument('--scale', '-s', default=100, type=int,
    help='Scaling factor (%%)')
  parser.add_argument('--lang', '-l', default='deu_frak',
    help='Tesseract language model')
  nargs = parser.parse_args()
  args = vars(nargs)
  convert(args['imgdir'], args['workdir'], args['lang'], args['scale'])

if __name__ == '__main__':
  run()
