#!/bin/sh

echo "Bytes,L1 Misses, L2 Misses, L3 Misses, Total Instructions, Time (s);";

fileName="mmult_matB_only1"
size=1000

a=0

while [ $a -lt 8 ]
do
   ./$fileName $size

   a=`expr $a + 1`
done
