#!/bin/bash -l
#SBATCH --time=0:01:00
#SBATCH --mem=100G
#SBATCH --constraint=elves
module load OpenMPI
module load foss/2020a --quiet

echo pThread

time /homes/yswang/hw4/pthread/pthread10k
grep DATA *.out > 10kTimes.csv