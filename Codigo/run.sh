#!/bin/sh

echo "Bytes,L1 Misses, L2 Misses, L3 Misses, Total Instructions, Time (s);";

fileName="dif_mmult"
size=40

a=0

while [ $a -lt 8 ]
do
   ./$fileName $size

   a=`expr $a + 1`
done
