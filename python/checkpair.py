# -*- coding: utf8 -*-

import codecs, sys

def run(filename):
  print filename
  countDouble = 0
  countSingle = 0
  countLG = 0
  for l, line in enumerate(codecs.open(filename, 'r', 'utf8')):
    hasDouble = False
    hasSingle = False
    hasLG = False
    for c in line:
      if c == u'»':
        hasDouble = True
	countDouble += 1
      elif c == u'«':
        hasDouble = True
	countDouble -= 1
      elif c == u'›':
        hasSingle = True
	countSingle += 1
      elif c == u'‹':
        hasSingle = True
	countSingle -= 1
      elif c == u'>':
        hasLG = True
	countLG += 1
      elif c == u'<':
        hasLG = True
	countLG -= 1
    if hasSingle or hasDouble or hasLG:
      print 's:', countSingle, "d:", countDouble, "<:", countLG, 'line:', l,\
        line.encode('utf8'),
  print 'Single:', countSingle, 'Double:', countDouble, '<>:', countLG

if __name__ == '__main__':
  run(sys.argv[1])

