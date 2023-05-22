
#define MASTER 0

#include <mpi.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "ssort.h"

#define LEFT_NEIGHBOR(rank, size) ((rank) > 0 ? (rank) - 1 : (size) - 1)
#define RIGHT_NEIGHBOR(rank, size) ((rank) < (size) - 1 ? (rank) + 1 : 0)
#define FILENAME "output.txt"
// # mpirun -n 15 ./shearsort /proj/uppmax2023-2-13/nobackup/shear_indata/input15.txt /home/anle5400/pdp/ass/pds_indiv/trueSlurm_0.txt >> new.txt

int main(int argc, char **argv) {
	int rank, num_processors, N, remainder;
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
		// if (rank == MASTER){
		// 	clock_t begin = clock();
		// 	qsort(input, N, sizeof(double), compare);
		// 	clock_t end = clock();
		// 	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;			printf("Num: %i\n", N);
			
		// 	printf("Number: %i\n", N);
		// 	printf("\tSerial Time: %f\n\n", time_spent);
		// }
		// db_arr(input, N, "FILE IN", rank, num_processors);
	}

	int num_rows_recv;
	MPI_Bcast(&num_values, 1, MPI_INT, 0, MPI_COMM_WORLD);			

	/***
	* NEW (Variable scatter!)
	***/
	double *sort_tmp;
	int minRows = num_values/num_processors;
	num_rows_recv = (num_values % num_processors > 0) ? (rank < num_values % num_processors) ? (1 + minRows) : minRows : minRows;
	// Prepare Buffer to Receiv array
	num_elems_recv = num_rows_recv * num_values;
	if (NULL == (sort_tmp = malloc(num_elems_recv * sizeof(double)))) {
		perror("Couldn't allocate memory for output");
		return 2;
	}
	// Scatter Input Array
	MPI_Scatterv(input, num_elems_send, displs,
				MPI_DOUBLE, sort_tmp, num_elems_recv,
				MPI_DOUBLE,
				MASTER, MPI_COMM_WORLD);

	double start = MPI_Wtime();
	// Create pointer and run sort algorithm, returns pointer to final array of sorted to check
	int len = num_elems_recv;   // Should be n (where N = n*n)
    int phase = 0;

	// Create the datatype and Prepare buffers for alltoallV for later use
	MPI_Type_vector(num_rows_recv, 1, num_values, MPI_DOUBLE, &column_type);
	MPI_Type_commit(&column_type);
    MPI_Type_create_resized(column_type, 0, sizeof(double), &column_resized);
    MPI_Type_commit(&column_resized);

	atav_scount = malloc(sizeof(int) * num_processors);
	atav_sdispls = malloc(sizeof(int) * num_processors);
	atav_rcount = malloc(sizeof(int) * num_processors);
	atav_rdispls = malloc(sizeof(int) * num_processors);

	remainder = num_values % num_processors;
	num_rows_per_processor = num_values / num_processors;
	for (int i=0; i<num_processors; i++){
		if (remainder){
			atav_scount[i] = (num_rows_per_processor + 1);
			// atav_scount[i] = 1;
			atav_rcount[i] = (num_rows_per_processor + 1) * num_rows_recv;
			remainder -= 1;
		} else {
			atav_scount[i] = num_rows_per_processor;
			// atav_scount[i] = 1;
			atav_rcount[i] = num_rows_per_processor * num_rows_recv;
		}
		if(i>0){
			atav_sdispls[i] = atav_sdispls[i-1] + atav_scount[i-1];
			atav_rdispls[i] = atav_rdispls[i-1] + atav_rcount[i-1];
		} else {
			atav_sdispls[i] = 0;
			atav_rdispls[i] = 0;
		}
	}

	db_arr(sort_tmp, len, "Received", rank, phase);
	output = malloc(sizeof(double)*N);
	sort(sort_tmp, &len, phase, output, MPI_COMM_WORLD);
	// db_arr(sort_tmp, len, "afterSort", rank, phase);
	

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
        db_arr(output, N, "OUT", rank, phase);

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

void sort(double *arr, int *len, int phase, double *output, MPI_Comm communicator) {
	int rank, num_processors;

	// Narr = New array
	// Buffers
	double *Narr;
    Narr = prep_buffs(*len);

	MPI_Comm_size(communicator, &num_processors); 
	MPI_Comm_rank(communicator, &rank);

	// Only sort when there's existing doubles in the array
	if (phase < num_values && phase % 2 == 0) {
        if (rank%2==0){
    		qsort(arr, *len, sizeof(double), compare);
        } else {
    		qsort(arr, *len, sizeof(double), compareOpp);
        }
        exchange_Numbers(arr, len, rank, phase, Narr);
        arr = Narr;
	} else if (phase < num_values && phase % 2 == 1) {
        qsort(arr, *len, sizeof(double), compare);
        exchange_Numbers(arr, len, rank, phase, Narr);
        arr = Narr;
    } else if (phase == num_values) {
        if (phase%2==1){
            exchange_Numbers(arr, len, rank, phase, Narr);
            arr = Narr;
        } 
        qsort(arr, *len, sizeof(double), compare);
        // db_arr(arr, *len, "PreFinal", rank, phase);
		gather_Numbers(arr, *len, output, MPI_COMM_WORLD);

		// printf("\n");
        return;
    }
    // MAIN DEBUGGER
    phase += 1;
    // db_arr(arr, *len, "Post Phase", rank, phase);

    // Call recursively
    sort(arr, len, phase, output, communicator);
    return;
}

void exchange_Numbers(double *arr, int *len, int rank, int phase, double *Narr){
	// Buffers
    double *swapTmp;
	swapTmp = prep_buffs(*len);
	MPI_Barrier( MPI_COMM_WORLD );

    // MPI_Alltoall(arr, 1, MPI_DOUBLE,
    //              Narr, 1, MPI_DOUBLE,
    //              MPI_COMM_WORLD);
	MPI_Alltoallv(arr, atav_scount,
				atav_sdispls, column_resized, Narr,
				atav_rcount, atav_rdispls, MPI_DOUBLE,
				MPI_COMM_WORLD);

    swapTmp = arr;
    arr = Narr;
    Narr = swapTmp;
    free(Narr);
	//DEBUG
	db_arr(arr, *len, "After Exchange", rank, phase);
	return;
};

void gather_Numbers(double *arr_in, int len, double *output, MPI_Comm communicator){
	int rank, num_processors;
	
	MPI_Comm_size(communicator, &num_processors); 
	MPI_Comm_rank(communicator, &rank);

	// Prepare Buffers to Gather
	int *finLens = malloc(sizeof(int)*num_processors);
	int outlen = len;
	int *displs = malloc(sizeof(int) * num_processors);
	MPI_Gather(&outlen, 1, MPI_INT,
               finLens, 1, MPI_INT, MASTER, MPI_COMM_WORLD);
	// Settle Displs and counts
	if (rank==MASTER){
		for (int i = 0; i<num_processors;i++){
			if(i>0){
				displs[i] = displs[i-1] + finLens[i-1];
			} else {
				displs[i] = 0;
			}
		}
	}

	MPI_Gatherv(arr_in, outlen, MPI_DOUBLE,
					output, finLens, displs,
					MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
	
	return;
}

double median(double *arr, int len){
	if (len % 2 == 0) {
		return (arr[len/2 - 1] + arr[len/2]) / 2.0;
	} else {
		return arr[len/2];
	}
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
			// Print Integers for CHECKA3!
			if (0 > fprintf(file, "%.6f", output[i])) {
				perror("Couldn't write to output file");
			}
		} else {
			if (0 > fprintf(file, "%.6f ", output[i])) {
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
