#!/bin/bash -l

#SBATCH -M snowy
#SBATCH -A uppmax2023-2-13
#SBATCH -t 40:00

# backwards10.txt          backwards4.txt       input10.txt          input250000000.txt  input500000000.txt
# backwards125000000.txt   backwards93.txt      input125000000.txt   input3.txt          input93.txt
# backwards2000000000.txt  input1000000000.txt  input2000000000.txt  input4.txt          output10.txt

# mpirun -n 16 ./quicksort /proj/uppmax2023-2-13/nobackup/qsort_indata/input125000000.txt /home/anle5400/ass/pdp_A3/trueSlurm_0.txt 2
# mpirun -n 4 ./quicksort /proj/uppmax2023-2-13/nobackup/qsort_indata/input10.txt /home/anle5400/ass/pdp_A3/trueSlurm_0.txt 3

fileidx=$1

if [[ $fileidx == 0 ]]
then
    mpirun ./quicksort /proj/uppmax2023-2-13/nobackup/qsort_indata/input125000000.txt /home/anle5400/pdp/ass/pdp_A3/trueSlurm.txt $2
elif [[ $fileidx == 1 ]]
then
    mpirun ./quicksort /proj/uppmax2023-2-13/nobackup/qsort_indata/input250000000.txt /home/anle5400/pdp/ass/pdp_A3/trueSlurm.txt $2
elif [[ $fileidx == 2 ]]
then
    mpirun ./quicksort /proj/uppmax2023-2-13/nobackup/qsort_indata/input500000000.txt /home/anle5400/pdp/ass/pdp_A3/trueSlurm.txt $2
elif [[ $fileidx == 3 ]]
then
    mpirun ./quicksort /proj/uppmax2023-2-13/nobackup/qsort_indata/input1000000000.txt /home/anle5400/pdp/ass/pdp_A3/trueSlurm.txt $2
elif [[ $fileidx == 4 ]]
then
    mpirun ./quicksort /proj/uppmax2023-2-13/nobackup/qsort_indata/backwards125000000.txt /home/anle5400/pdp/ass/pdp_A3/trueSlurm.txt $2
elif [[ $fileidx == 5 ]]
then
    mpirun ./quicksort /proj/uppmax2023-2-13/nobackup/qsort_indata/input2000000000.txt /home/anle5400/pdp/ass/pdp_A3/trueSlurm.txt $2
elif [[ $fileidx == 6 ]]
then
    mpirun ./quicksort /proj/uppmax2023-2-13/nobackup/qsort_indata/backwards2000000000.txt /home/anle5400/pdp/ass/pdp_A3/trueSlurm.txt $2
elif [[ $fileidx == 7 ]]
then
    mpirun ./quicksort /proj/uppmax2023-2-13/nobackup/qsort_indata/backwards125000000.txt /home/anle5400/pdp/ass/pdp_A3/trueSlurm.txt $2
elif [[ $fileidx == 8 ]]
then
    mpirun ./quicksort /proj/uppmax2023-2-13/nobackup/qsort_indata/backwards10.txt /home/anle5400/pdp/ass/pdp_A3/trueSlurm.txt $2
fi
