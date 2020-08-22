#!/usr/bin/env python

import os, subprocess, shutil, sys, tempfile

def usage(cmd):
  print("{0} indir outdir [scale]\n".format(cmd))
  print(" default scale: 50\n")

def main():
  if len(sys.argv) < 3:
    usage(sys.argv[0])
    return
  path = sys.argv[1]
  tgtDir = sys.argv[2]
  scale = 50.0
  if len(sys.argv) > 3:
    scale = float(sys.argv[3])
  tmpDir = tempfile.mkdtemp()
  print("Temp dir: {}".format(tmpDir))
  print("Scale: {}".format(scale))
  i = 0
  for f in sorted(os.listdir(path)):
    print(f)
    if f[-4:] != '.tif': continue
    i += 1
    cmd = ['convert', "-scale", "%d%%" % scale,
      "-depth", "2", "-colors", "4", "-type", "Palette",
      "%s/%s" % (path, f), "%s/%03d.png" % (tmpDir,i)]
    print(" ".join(cmd))
    subprocess.call(cmd)
    cmd =['pngcrush', "-d", tgtDir,
      "%s/%03d.png" % (tmpDir, i)]
    print(" ".join(cmd))
    subprocess.call(cmd)
#  subprocess.call(['pngnq', "-n", "4", "-f", "-d", "../pngs", "-e", ".png",
#    "rename/%03d.png" % i])
  shutil.rmtree(tmpDir)


if __name__ == '__main__':
  main()
