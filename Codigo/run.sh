#!/bin/sh

fileName="mmult_t"
echo "Bytes;L1 Misses;L2 Misses;L3 Misses;Total Instructions;RAM Accesses p/ Instruction;Time (s);FLOP;GFLOPS;Operational Intensity;" > ../Resultados/$fileName.csv
./$fileName >> ../Resultados/$fileName.csv
