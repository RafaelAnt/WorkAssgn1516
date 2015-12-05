#!/bin/sh

fileName="mmult_matB_only1_BxA"
echo "Bytes;L1 Misses;L2 Misses;L3 Misses;Total Instructions;RAM Accesses p/ Instruction;Time (s);FLOP;GFLOPS;Operational Intensity;" > ../Resultados/$fileName.csv
./$fileName >> ../Resultados/$fileName.csv
