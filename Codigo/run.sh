#!/bin/sh

echo "Bytes,L1 Misses, L2 Misses, L3 Misses, Total Instructions, Time (s), FLOP, GFLOPS, Operational Intensity;";

fileName="mmult"
size=40

a=0

while [ $a -lt 8 ]
do
   ./$fileName $size

   a=`expr $a + 1`
done

echo ";"

size=120

a=0

while [ $a -lt 8 ]
do
   ./$fileName $size

   a=`expr $a + 1`
done

size=600

a=0

while [ $a -lt 8 ]
do
   ./$fileName $size

   a=`expr $a + 1`
done

size=1000

a=0

while [ $a -lt 8 ]
do
   ./$fileName $size

   a=`expr $a + 1`
done
