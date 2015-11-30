#!/bin/sh

echo start test;

fileName="mmult_Orig"
size=40

a=0

while [ $a -lt 8 ]
do
   ./$fileName $size

   a=`expr $a + 1`
done
