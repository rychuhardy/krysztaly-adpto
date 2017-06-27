#!/bin/bash
echo $PWD
"./cmake-build-debug/AdPTO" < test/$1 > test/$1.out

python resources/judge/judge.py test/$1 test/$1.out