#!/bin/sh

fileName="mmult_ijk"
echo "Bytes;L1 Misses;L2 Misses;L3 Misses;Total Instructions;RAM Accesses p/ Instruction;Time (s);FLOP;GFLOPS;Operational Intensity;" > $fileName.csv
./$fileName >> $fileName.csv
