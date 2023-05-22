
#define MASTER 0

#include <mpi.h>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "ssort.h"

#define LEFT_NEIGHBOR(rank, size) ((rank) > 0 ? (rank) - 1 : (size) - 1)
#define RIGHT_NEIGHBOR(rank, size) ((rank) < (size) - 1 ? (rank) + 1 : 0)
#define FILENAME "output.txt"
// # mpirun -n 15 ./shearsort /proj/uppmax2023-2-13/nobackup/shear_indata/input15.txt /home/anle5400/pdp/ass/pds_indiv/trueSlurm_0.txt >> new.txt

int main(int argc, char **argv) {
	int remainder;
	MPI_Status status;
	double longest_exec_time;
	char *input_name = argv[1];
	char *output_name = argv[2];
	/***
	*******************************************************************************************************
	***/
	double *input, *output;

	if (3 != argc) {
		printf("Usage: shearsort input_file output_file\n");
		return 1;
	}

	MPI_Init(&argc, &argv);               
	MPI_Comm_size(MPI_COMM_WORLD, &num_processors); 
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	// Buffers for gather
	gfinLens = malloc(sizeof(int)*num_processors);
	gdispls = malloc(sizeof(int) * num_processors);


	// Read Inputs from file and calculate num per processor and Bcast num per process
	int *num_elems_send = malloc(sizeof(int) * num_processors);
	int *displs = malloc(sizeof(int) * num_processors);
	if (rank==MASTER){
		if (0 > (num_values = read_input(input_name, &input))) {
			return 2;
		}
		N = num_values * num_values;
		remainder = num_values % num_processors;
		num_rows_per_processor = num_values / num_processors;
		for (int i=0; i<num_processors; i++){
			if (remainder){
				num_elems_send[i] = (num_rows_per_processor + 1) * num_values;
				remainder -= 1;
			} else {
				num_elems_send[i] = num_rows_per_processor * num_values;
			}
			if(i>0){
				displs[i] = displs[i-1] + num_elems_send[i-1];
			} else {
				displs[i] = 0;
			}
		}
	}

	MPI_Bcast(&num_values, 1, MPI_INT, 0, MPI_COMM_WORLD);			

	/***
	* Variable scatter! Compute number of rows to receive
	***/
	double *sort_tmp;
	tmp = prep_buffs(num_values*num_values);
	int minRows = num_values/num_processors;
	num_rows_recv = (num_values % num_processors > 0) ? (rank < num_values % num_processors) ? (1 + minRows) : minRows : minRows;
	
	// Prepare Buffer to Receiv array
	num_elems_recv = num_rows_recv * num_values;
	if (NULL == (sort_tmp = malloc(num_elems_recv * sizeof(double)))) {
		perror("Couldn't allocate memory for output");
		return 2;
	}

	// Scatter Input Array
	MPI_Scatterv(input, num_elems_send, displs, MPI_DOUBLE, 
				sort_tmp, num_elems_recv, MPI_DOUBLE,
				MASTER, MPI_COMM_WORLD);

	// Create pointer and run sort algorithm, returns pointer to final array of sorted to check
	int len = num_elems_recv;   // Should be n (where N = n*n)
    int phase = 0;

	// Create the datatype and Prepare buffers for alltoallV for later use
	MPI_Type_vector(num_values, 1, num_values, MPI_DOUBLE, &column_type);
	MPI_Type_commit(&column_type);
    MPI_Type_create_resized(column_type, 0, sizeof(double), &column_resized);
    MPI_Type_commit(&column_resized);

	atav_scount = malloc(sizeof(int) * num_processors);
	atav_sdispls = malloc(sizeof(int) * num_processors);

	remainder = num_values % num_processors;
	num_rows_per_processor = num_values / num_processors;

	isOdd = 0;
	
	for (int i=0; i<num_processors; i++){
		if (remainder){
			atav_scount[i] = (num_rows_per_processor + 1);
			remainder -= 1;
		} else {
			atav_scount[i] = num_rows_per_processor;
		}
		if(i>0){
			atav_sdispls[i] = atav_sdispls[i-1] + atav_scount[i-1];
		} else {
			atav_sdispls[i] = 0;
		}
		if (rank == i){
			startIndicator = isOdd;
		}
		if (atav_scount[i] % 2 == 1) {
			flip_isOdd();
		}

	}
	int outlen = len;
	MPI_Gather(&outlen, 1, MPI_INT,
               gfinLens, 1, MPI_INT, MASTER, MPI_COMM_WORLD);

	// Settle gather displs and counts
	if (rank==MASTER){
		for (int i = 0; i<num_processors;i++){
			if(i>0){
				gdispls[i] = gdispls[i-1] + gfinLens[i-1];
			} else {
				gdispls[i] = 0;
			}
		}
	}




	output = malloc(sizeof(double)*N);
	// Narr = New array
	// Buffers
	double *Narr;
    Narr = prep_buffs(len);
	double start = MPI_Wtime();

	stepsNeeded = ceil(log2(num_values))*2;
	printf("steps needed: %i", stepsNeeded);
	for (;phase<= num_values; phase++){
		sort(sort_tmp, &len, phase, output, Narr);
	}
	
	// // Calculate longest execution time
	double my_execution_time = MPI_Wtime() - start;
	MPI_Reduce(&my_execution_time, &longest_exec_time, 1, MPI_DOUBLE, MPI_MAX, MASTER, MPI_COMM_WORLD);
	
	if (rank==MASTER){
		write_output(output_name, output, N);
	}
	MPI_Finalize(); 

	// // Announce timing
	if (rank == MASTER) {
		printf("FileName: %s\n\tProcessors: %i\n\tTime: %f\n\n", input_name, num_processors, longest_exec_time);
		// printf("%f", longest_exec_time);
	}

	return 0;
}

int compare(const void* num1, const void* num2)
{  
    double a = *(double*) num1;  
    double b = *(double*) num2;  
    return a > b ? 1
				: -1;
}  

int compareOpp(const void* num1, const void* num2)
{  
    double a = *(double*) num1;  
    double b = *(double*) num2;  
    return a > b ? -1
				: 1;
}  

void sort(double *arr, int *len, int phase, double *output, double *Narr) {
	if (phase != stepsNeeded) {
		int beginwithOdd = startIndicator;
		for (int i=0; i<num_rows_recv; i++){
			if (phase < stepsNeeded && phase % 2 == 0){
				if (beginwithOdd == 0) {
					qsort(&arr[i*num_values], num_values, sizeof(double), compare);
				} else {
					qsort(&arr[i*num_values], num_values, sizeof(double), compareOpp);
				}
				beginwithOdd = beginwithOdd ? 0 : 1;
			} else if (phase < stepsNeeded && phase % 2 == 1) {
				qsort(&arr[i*num_values], num_values, sizeof(double), compare);
			}
		}
		// db_arr(arr, *len, "Post qsort", rank, phase);
		exchange_Numbers(arr, len, rank, phase, Narr);
		// arr = Narr;
		// memcpy(Narr, arr, sizeof(double)**len);
		memcpy(arr, Narr, sizeof(double)**len);
	} else if (phase == stepsNeeded) {
        if (phase%2==1){
            exchange_Numbers(arr, len, rank, phase, Narr);
            // arr = Narr;
			memcpy(arr, Narr, sizeof(double)**len);
        } 
		// Comment the following to output in snakelike order
		// for (int i=0; i<num_rows_recv; i++){
		// 	qsort(&arr[i*num_values], num_values, sizeof(double), compare);
		// }
		gather_Numbers(arr, *len, output, MPI_COMM_WORLD);

        return;
    }
	// memcpy(sort_tmp, arr, sizeof(double)**len);
	sort_tmp = arr;
    return;
}

// Total Exchange or Transpose
void exchange_Numbers(double *arr, int *len, int rank, int phase, double *Narr){
	// Buffers
	gather_Numbers(arr, *len, tmp, MPI_COMM_WORLD);
	// Gather

	// MPI_Scatter(tmp, num_rows_per_processor, column_resized, 
	// 		Narr, num_elems_recv, MPI_DOUBLE,
	// 		MASTER, MPI_COMM_WORLD);

	// Scatter new row to be sorted
	MPI_Scatterv(tmp, atav_scount, atav_sdispls, column_resized, 
				Narr, num_elems_recv, MPI_DOUBLE,
				MASTER, MPI_COMM_WORLD);
	return;
};

void flip_isOdd(){
	isOdd = (isOdd) ? 0 : 1;
}

void gather_Numbers(double *arr_in, int len, double *output, MPI_Comm communicator){

	// Prepare Buffers to Gather
	// MPI_Gather(arr_in, len, MPI_DOUBLE,
    //            output, len, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);

	MPI_Gatherv(arr_in, len, MPI_DOUBLE,
					output, gfinLens, gdispls,
					MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
	
	return;
}


void db_arr(double *ptr, int len, char *msg, int rank, int npPerGroup){
	printf("%s : %i %i :: ", msg, rank, npPerGroup);
	for (int i=0; i<len; i++){
		printf("%lf ", ptr[i]);
	}
	printf("\n");
	return;
}

double *prep_buffs(int newSize){
	double *prep_buff;
	if (NULL == (prep_buff = malloc(newSize * sizeof(double)))) {
		perror("Couldn't allocate memory for output");
	}
	
	return prep_buff;
}

int read_input(const char *file_name, double **values) {
	FILE *file;
	if (NULL == (file = fopen(file_name, "r"))) {
		perror("Couldn't open input file");
		return -1;
	}
	if (EOF == fscanf(file, "%d", &num_values)) {
		perror("Couldn't read element count from input file");
		printf("AT: %i", num_values);
		return -1;
	}
	if (NULL == (*values = malloc(num_values * num_values * sizeof(double)))) {
		perror("Couldn't allocate memory for input");
		return -1;
	}
	for (int i=0; i< num_values * num_values; i++) {
		if (EOF == fscanf(file, "%lf", &((*values)[i]))) {
			perror("Couldn't read elements from input file");
			return -1;
		}
	}
	if (0 != fclose(file)){
		perror("Warning: couldn't close input file");
	}
	return num_values;
}


int write_output(char *file_name, const double *output, int num_values) {
	FILE *file;
	if (NULL == (file = fopen(file_name, "w"))) {
		perror("Couldn't open output file");
		return -1;
	}
	for (int i = 0; i < num_values; i++) {
		if (i == num_values - 1) {
			// Print Integers to check for correctness!
			if (0 > fprintf(file, "%.0f", output[i])) {
				perror("Couldn't write to output file");
			}
		} else {
			if (0 > fprintf(file, "%.0f ", output[i])) {
				perror("Couldn't write to output file");
			}
		}
	}
	if (num_values <=0 ) {
		if (0 > fprintf(file, "\n")) {
			perror("Couldn't write to output file");
		}
	}
	if (0 != fclose(file)) {
		perror("Warning: couldn't close output file");
	}

	return 0;
}
