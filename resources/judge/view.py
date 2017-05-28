#!/usr/bin/python

import sys
import os
import Image
import ImageDraw


def readBoard(fname):
  f=open(fname)
  s = f.readline()
  (W,K) = s.split()    
  W = int(W)
  K = int(K)
  N = int(f.readline())
  print W,K,N
  t={}
  t["W"]=W
  t["K"]=K
  t["N"]=N
  for w in range(W):
    for k in range(K): t[w,k]=' '
  for w in range(W):
    s=f.readline()
    for k in range(K):
      t[w,k]=s[k]
    # end for
  # end for
  lk=0
  lz=0
  for w in range(W):
    for k in range(K): 
      if t[w,k]=='*': lk+=1
      if t[w,k] in '\\/': lz+=1
    # end for
  # end fog
  print "krysztalow",lk
  print "zwierciadel",lz


  return t
# end def   

def drawBoard(t):
  W = t["W"]
  K = t["K"]
  N = t['N']

#  imgs = loadImages()

  blok = Image.open( "img/blok.png" )
  cel = Image.open( "img/cel.png" )
  lu1 = Image.open( "img/lustro-1.png" )
  lu2 = Image.open( "img/lustro-2.png" )
  pion = Image.open( "img/pion.png" )
  poziom = Image.open( "img/poziom.png" )

  boardImg = Image.new( "RGB", (26*K,26*W) )
  draw = ImageDraw.Draw( boardImg )


  # wypelnij plansze urzadzeniami
  for w in range(W):
    for k in range(K):
      if t[w,k]=='#': boardImg.paste( blok, (k*26,w*26) )
      elif t[w,k]=="*": boardImg.paste( cel, (k*26,w*26) )
      elif t[w,k]=="\\": boardImg.paste( lu1, (k*26,w*26) )
      elif t[w,k]=="/": boardImg.paste( lu2, (k*26,w*26) )

  # bieg promienia
  
  l1 = {'E':'S','W':'N','S':'E','N':'W'}
  l2 = {'E':'N','W':'S','S':'W','N':'E'}
  w=1
  k=0
  di='E'
  while True:
    if t[w,k]==' ' and di in 'WE' : boardImg.paste( poziom, (k*26,w*26) )
    elif t[w,k]==' ' and di in 'NS' : boardImg.paste( pion, (k*26,w*26) )
    elif t[w,k]=='#': break
    elif t[w,k]=='\\': di=l1[di]
    elif t[w,k]=='/': di=l2[di]
    
    
    if di=='E': k+=1
    elif di=='W': k-=1
    elif di=='N': w-=1
    elif di=='S': w+=1
    
  # end  
  
  boardImg.save(fname+".png")

  # wstawic wlasciwy program do wyswietlania plikow .png

#  os.system("eog "+fname+".png")
#  os.system("display "+fname+".png")

# end def

##########################################
#
# program glowny


if len(sys.argv)!=2:
  print "Usage: view <file>"
else:
  fname=sys.argv[1]
  t=readBoard(fname)
  drawBoard(t)
