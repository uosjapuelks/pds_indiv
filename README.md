# Individual Project: 6.3 Shear Sort

Runs on UPPMAX servers and requires initial loading of `gcc` and `openmpi` modules by running the following:
`module load gcc openmpi`

## Getting Started
### Provided files

* `ssort.c`: Main shear sort code written in C
* `ssort.h`: Header file for `ssort.c`
* `Makefile`: Makefile to compile `ssort.c`
* `Proj_Report.pdf`: Project Report

### Building the project


```
make        # Builds project using mpicc
make code   # Functions the same as make
make all    # Functions the same as make
make debug  # Builds project with debug symbols
make clean  # Cleans project directory
```

### Testing Code
Include the following into a shell script:
```
#SBATCH -M snowy
#SBATCH -A uppmax2023-2-13
#SBATCH -t [timeout]
```

After building, we can use the executable `shear` that is produced in the following way in the shell script:
`mpirun ./shear [INPUT_FILE] [OUTPUT_FILE]`

Run using the NUMBER_OF_PROCESSORS desired:
`sbatch -p node -n [NUMBER_OF_PROCESSORS] [YOUR_SHELL_SCRIPT]`

The programme would run the shear sort algorithm on the data from the `INPUT_FILE` and output the array in a snakelike manner, a characteristic of shear sort into `OUTPUT_FILE`. A double will be printed on the terminal which is the MPI walltime from running the sort algorithm.