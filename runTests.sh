#!/bin/bash -l
#SBATCH -M snowy
#SBATCH -A uppmax2023-2-13
#SBATCH -t 40:00

# 0 1 2 3 5 (doubles prev with each inc)
# (125 250 500 1000 2000)* 1000000
# module load gcc/8.2.0 openmpi/3.1.1

# sbatch -p node -n 1 a3.sh 0 1
# sbatch -p node -n 16 a3.sh 1 1
# sbatch -p node -n 1 a3.sh 2 1
# sbatch -p node -n 1 a3.sh 3 1
# sbatch -p node -n 1 a3.sh 4 1
# sbatch -p node -n 1 a3.sh 5 1
# sbatch -p node -n 1 a3.sh 6 1

# sbatch -p node -n 16 a3.sh 0 2
# sbatch -p node -n 16 a3.sh 1 2
# sbatch -p node -n 4 a3.sh 2 2
# sbatch -p node -n 4 a3.sh 3 2
# sbatch -p node -n 4 a3.sh 4 2
# sbatch -p node -n 4 a3.sh 5 2
# sbatch -p node -n 4 a3.sh 6 2

# sbatch -p node -n 16 a3.sh 0 3
# sbatch -p node -n 16 a3.sh 1 3
# sbatch -p node -n 4 a3.sh 2 3
# sbatch -p node -n 4 a3.sh 3 3
# sbatch -p node -n 4 a3.sh 4 3
# sbatch -p node -n 4 a3.sh 5 3
# sbatch -p node -n 4 a3.sh 6 3

# Strong Scaling
# sbatch -p node -n 1 a3.sh 1 1
# sbatch -p node -n 2 a3.sh 1 1
# sbatch -p node -n 4 a3.sh 1 1
# sbatch -p node -n 8 a3.sh 1 1
# sbatch -p node -n 16 a3.sh 1 1
# sbatch -p node -n 32 a3.sh 1 1

# Weak Scaling
# sbatch -p node -n 2 a3.sh 0 3
# sbatch -p node -n 4 a3.sh 1 3
# sbatch -p node -n 8 a3.sh 2 3
# sbatch -p node -n 16 a3.sh 3 3
# sbatch -p node -n 32 a3.sh 5 2

# Check A3
sbatch -p node -n 10 a4.sh 7
sbatch -p node -n 1 a4.sh 7
