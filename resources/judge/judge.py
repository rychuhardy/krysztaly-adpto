#!/usr/bin/python

import sys
import os


def readBoard(fname):
  try:
    f=open(fname)
    s = f.readline()
    (W,K) = s.split()    
    W = int(W)
    K = int(K)
    N = int(f.readline())
#    print "wymiary",W,K,N
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
    f.close()
    return t
  except:
    err(1)
  # end try
# end def   

def spr(ti,to):
  W = ti["W"]
  K = ti["K"]
  N = ti['N']

  # zgodnosc danych i wynikow

  if to["W"]!=W or to["K"]!=K or to["N"]!=N: err(2)

  for w in range(W):
    for k in range(K): 
      if ti[w,k]=='#' and to[w,k]!='#': err(2)
      if ti[w,k]=='*' and to[w,k]!='*': err(2)
      if ti[w,k]==' ' and not to[w,k] in ' /\\': err(2)
    # end for
  # end for

  # liczba zwierciadel w wynikach

  lz=0
  for w in range(W):
    for k in range(K): 
      if to[w,k] in "\\/": lz+=1

#  print lz,"zwierciadel"
  if lz>N: err(3)

  # bieg promienia i usuwanie krysztalow
  
  l1 = {'E':'S','W':'N','S':'E','N':'W'}
  l2 = {'E':'N','W':'S','S':'W','N':'E'}
  w=1
  k=0
  di='E'
  while True:
    if to[w,k]=='#': break
    elif to[w,k]=='*': to[w,k]=' '
    elif to[w,k]=='\\': di=l1[di]
    elif to[w,k]=='/': di=l2[di]
        
    if di=='E': k+=1
    elif di=='W': k-=1
    elif di=='N': w-=1
    elif di=='S': w+=1
    
  # end while  

  # liczba pozostalych krysztalow

  lk=0
  for w in range(W):
    for k in range(K): 
      if to[w,k]=='*': lk+=1

#  print "pozostalo",lk,"krysztalow"
  if lk>0: err(4)
    
  print "OK"
  sys.exit(0)  
    
# end def

##########################################

def err(n):
  error = { 1:"bledny format pliku",
            2:"niezgodne dane i wyniki",
            3:"zbyt wiele zwierciadel",
            4:"nieoswietlone wszystkie krysztaly" }
  print "Error",n,":",error[n]
  sys.exit(n)
# end def

##########################################
#
# program glowny

if len(sys.argv)!=3:
  print "Usage: ./judge.py <file.in> <file.out>"
else:
  finame=sys.argv[1]
  ti=readBoard(finame)
  foname=sys.argv[2]
  to=readBoard(foname)
  spr(ti,to)
# end if













































































































































































































# nienajgorszy ten nieicon...
