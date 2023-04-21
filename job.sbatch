#!/bin/bash

#SBATCH --job-name=pingpong
#SBATCH --nodes=2
#SBATCH --cpus-per-task=1
#SBATCH --mem=2GB 
#SBATCH --time=01:00:00 
#SBATCH --verbose

module purge 
module load openmpi/gcc/4.0.5 intel/19.1.2 openmpi/intel/4.0.5 

mpiexec ./pingpong 0 1
