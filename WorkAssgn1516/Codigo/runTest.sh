#!/bin/sh

echo start test;

fileName="mmult_Orig"

a=0

while [ $a -lt 8 ]
do
   ./$fileName

   a=`expr $a + 1`
done
