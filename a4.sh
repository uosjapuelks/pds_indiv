#!/bin/bash -l

#SBATCH -M snowy
#SBATCH -A uppmax2023-2-13
#SBATCH -t 40:00

# input1000.txt  input1600.txt  input3200.txt  input4.txt    input800.txt    output1500.txt  output3000.txt  output500.txt
# input1500.txt  input2000.txt  input3.txt     input500.txt  input_gen.py    output15.txt    output3.txt     output5.txt
# input15.txt    input3000.txt  input400.txt   input5.txt    output1000.txt  output2000.txt  output4.txt

# mpirun -n 2 ./shearsort /proj/uppmax2023-2-13/nobackup/shear_indata/input4.txt /home/anle5400/pdp/ass/pds_indiv/trueSlurm_0.txt >> new.txt
# mpirun -n 3 ./shearsort /proj/uppmax2023-2-13/nobackup/shear_indata/input4.txt /home/anle5400/pdp/ass/pds_indiv/trueSlurm_0.txt >> new.txt
# mpirun -n 4 ./shearsort /proj/uppmax2023-2-13/nobackup/shear_indata/input15.txt /home/anle5400/pdp/ass/pds_indiv/trueSlurm_0.txt >> new.txt
# Check Correctness
# /proj/uppmax2023-2-13/nobackup/shear_indata/output15.txt

fileidx=$1

if [[ $fileidx == 0 ]]
then
    mpirun ./shearsort /proj/uppmax2023-2-13/nobackup/shear_indata/input400.txt /home/anle5400/pdp/ass/pds_indiv/trueSlurm.txt
elif [[ $fileidx == 1 ]]
then
    mpirun ./shearsort /proj/uppmax2023-2-13/nobackup/shear_indata/input500.txt /home/anle5400/pdp/ass/pds_indiv/trueSlurm.txt
elif [[ $fileidx == 2 ]]
then
    mpirun ./shearsort /proj/uppmax2023-2-13/nobackup/shear_indata/input800.txt /home/anle5400/pdp/ass/pds_indiv/trueSlurm.txt
elif [[ $fileidx == 3 ]]
then
    mpirun ./shearsort /proj/uppmax2023-2-13/nobackup/shear_indata/input1000.txt /home/anle5400/pdp/ass/pds_indiv/trueSlurm.txt
elif [[ $fileidx == 4 ]]
then
    mpirun ./shearsort /proj/uppmax2023-2-13/nobackup/shear_indata/input1500.txt /home/anle5400/pdp/ass/pds_indiv/trueSlurm.txt
elif [[ $fileidx == 5 ]]
then
    mpirun ./shearsort /proj/uppmax2023-2-13/nobackup/shear_indata/input1600.txt /home/anle5400/pdp/ass/pds_indiv/trueSlurm.txt
elif [[ $fileidx == 6 ]]
then
    mpirun ./shearsort /proj/uppmax2023-2-13/nobackup/shear_indata/input2000.txt /home/anle5400/pdp/ass/pds_indiv/trueSlurm.txt
elif [[ $fileidx == 7 ]]
then
    mpirun ./shearsort /proj/uppmax2023-2-13/nobackup/shear_indata/input3000.txt /home/anle5400/pdp/ass/pds_indiv/trueSlurm.txt
elif [[ $fileidx == 8 ]]
then
    mpirun ./shearsort /proj/uppmax2023-2-13/nobackup/shear_indata/input3200.txt /home/anle5400/pdp/ass/pds_indiv/trueSlurm.txt
fi
