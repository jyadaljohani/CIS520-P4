#!/bin/bash -l
#SBATCH --time=0:01:00
#SBATCH --mem=100G
#SBATCH --constraint=elves
for i in 1 2 4 8 16
do
	echo "Tasks: $i"
	sbatch --constraint=elves --ntasks-per-node=$i --nodes=1 --job-name=pthread -o $i-core-10k.out pthread10k.sh
done