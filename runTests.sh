#!/bin/bash -l
#SBATCH -M snowy
#SBATCH -A uppmax2023-2-13
#SBATCH -t 5:00

# 0 1 2 3 4 5 6 7 8
# 400 500 800 1000 1500 1600 2000 3000 3200
# module load gcc/8.2.0 openmpi/3.1.1
# sbatch -p node -n 10 a4.sh 7

# Check A3
# Strong scale
# sbatch -p node -n 1 a4.sh 8
# sbatch -p node -n 2 a4.sh 8
# sbatch -p node -n 4 a4.sh 8
# sbatch -p node -n 8 a4.sh 8
# sbatch -p node -n 16 a4.sh 8
# sbatch -p node -n 32 a4.sh 8

# Weak scale
# sbatch -p node -n 2 a4.sh 0
# sbatch -p node -n 3 a4.sh 1
# sbatch -p node -n 8 a4.sh 2
# sbatch -p node -n 12 a4.sh 3
# sbatch -p node -n 32 a4.sh 5
