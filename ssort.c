
#define MASTER 0

#include <mpi.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "ssort.h"

#define LEFT_NEIGHBOR(rank, size) ((rank) > 0 ? (rank) - 1 : (size) - 1)
#define RIGHT_NEIGHBOR(rank, size) ((rank) < (size) - 1 ? (rank) + 1 : 0)
#define FILENAME "output.txt"
// # mpirun -n 4 ./quicksort /proj/uppmax2023-2-13/nobackup/qsort_indata/input125000000.txt /home/anle5400/pdp/ass/pdp_A3/trueSlurm_0.txt 2

int main(int argc, char **argv) {
	int rank, num_processors, N;
	MPI_Status status;
	double longest_exec_time;

	char *input_name = argv[1];
	char *output_name = argv[2];
	int strat = atoi(argv[3]);
	/***
	*******************************************************************************************************
	***/
	double *input, *output;

	if (4 != argc) {
		printf("Usage: qsort input_file output_file pivot_strat\n");
		return 1;
	}

	MPI_Init(&argc, &argv);               
	MPI_Comm_size(MPI_COMM_WORLD, &num_processors); 
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	// Read Inputs from file and calculate num per processor and Bcast num per process
	int *num_elems_send = malloc(sizeof(int) * num_processors);
	int *displs = malloc(sizeof(int) * num_processors);
	if (rank==MASTER){
		int num_values;
		if (0 > (num_values = read_input(input_name, &input))) {
			return 2;
		}
		N = num_values;
		int remainder = N % num_processors;
		num_elems_per_processor = N / num_processors;
		for (int i=0; i<num_processors; i++){
			if (remainder){
				num_elems_send[i] = num_elems_per_processor + 1;
				remainder -= 1;
			} else {
				num_elems_send[i] = num_elems_per_processor;
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

	int num_elems_recv;
	MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);			

	/***
	* NEW (Variable scatter!)
	***/
	double *sort_tmp;
	num_elems_recv = (N % num_processors > 0) ? (rank < N % num_processors) ? 1 + (N/num_processors) : N/num_processors : N / num_processors;
	// Prepare Buffer to Receiv array
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
    int phase = num_processors + 1;
	double * finalout = sort(sort_tmp, &len, phase, MPI_COMM_WORLD);

	// Prepare Buffers to Gather
	int *finLens = malloc(sizeof(int)*num_processors);
	int outlen = len;
	output = malloc(sizeof(double)*N);
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

	MPI_Gatherv(finalout, outlen, MPI_DOUBLE,
					output, finLens, displs,
					MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
	// // Calculate longest execution time
	double my_execution_time = MPI_Wtime() - start;
	MPI_Reduce(&my_execution_time, &longest_exec_time, 1, MPI_DOUBLE, MPI_MAX, MASTER, MPI_COMM_WORLD);
	
	// if (rank==MASTER){
	// 	// db_arr(output, N, "Final", rank, num_processors); /* Debugger */
	// 	write_output(output_name, output, N);
	// }
	MPI_Finalize(); 

	// // Announce timing
	if (rank == MASTER) {
		printf("FileName: %s\n\tProcessors: %i\n\tStrat: %i\n\tTime: %f\n\n", input_name, num_processors, strat, longest_exec_time);
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

double * sort(double *arr, int *len, int phase, MPI_Comm communicator) {
	int rank, num_processors;

	// Narr = New array
	// Buffers
	double *output, *Narr;

	MPI_Comm_size(communicator, &num_processors); 
	MPI_Comm_rank(communicator, &rank);

	// Only sort when there's existing doubles in the array
	if (*len > 0) {
        if (rank%2==0){
    		qsort(arr, *len, sizeof(double), compare);
        } else {
    		qsort(arr, *len, sizeof(double), compareOpp);
        }
		// MAIN DEBUGGER
		// db_arr(arr, *len, "Rank", rank, num_processors);
	}

	if (num_processors == 1) {
		return arr; // Finished! Time to Stitch
	} else {
		// Split communicator
		MPI_Comm halved_communicator;
		int npPerGroup = num_processors/2; 	// Number of proc per Grp of Communicators
		int color = (rank >= npPerGroup);		// Determines get big or small (1 = big, 0 = small)
		Narr = exchange_Numbers(arr, len, rank, color, npPerGroup, strat, communicator);

		MPI_Comm_split(communicator, color, rank, &halved_communicator);
		// Call recursively
		Narr = sort(Narr, len, halved_communicator);
		MPI_Comm_free(&halved_communicator);

		return Narr;
	}
}

double * exchange_Numbers(double *arr, int *len, int rank, int color, int npPerGroup, int strat, MPI_Comm communicator){
	double pivot, ave_med;
	double *sender, *recver, *kept_arr, *tmp_arr;
	double *medians;
	int sendNum, recvNum;
	int dest, tag;

	if (color){
		dest = rank - npPerGroup;
	} else {
		dest = rank + npPerGroup;
	}
	tag = rank % npPerGroup;

	double loc_med = 0;
	if (*len>0){
		loc_med = median(arr, *len);
	}
	switch (strat){
	case 1:	// Pick 1
		if (rank == MASTER) {
			pivot = median(arr, *len);
		}
		MPI_Bcast(&pivot, 1, MPI_DOUBLE, MASTER, communicator);
		break;

	case 2:	// Med of Med
		if (rank == MASTER) {
			medians = (double*)malloc(2 * npPerGroup * sizeof(double));
		}
		MPI_Gather(&loc_med, 1, MPI_DOUBLE, medians, 1, MPI_DOUBLE, 0, communicator);

		if (rank == MASTER) {
			pivot = median(medians, 2 * npPerGroup);
		}
		MPI_Bcast(&pivot, 1, MPI_DOUBLE, 0, communicator);
		break;

	case 3:	// Average
	    MPI_Reduce(&loc_med, &ave_med, 1, MPI_DOUBLE, MPI_SUM, 0, communicator);
		if (rank == MASTER) {
			pivot = ave_med / (2 * npPerGroup);
		}
		MPI_Bcast(&pivot, 1, MPI_DOUBLE, 0, communicator);			
		break;
	default:
		break;
	}

	int splitIdx=0;
	int startSendIdx = 0;
	int startRecvIdx = 0;
	for (int i=0; i<= *len;i++){
		splitIdx = i;		// Size of smaller than Median is i, bigger than Median is (len - i)
		
		if (i == *len){
			splitIdx=i;
			break;
		} else if (arr[i] >= pivot){
			break;
		}
	}

	if (color){		// Send Smaller Chunk (rank Bigger than Partner)
		sendNum = splitIdx;
		startSendIdx = 0;
		startRecvIdx = 0;
	} else {		// Send Bigger Chunk (rank Smaller than Partner)
		sendNum = *len - splitIdx;
		startSendIdx = splitIdx;
		startRecvIdx = sendNum;
	}
	MPI_Sendrecv(&sendNum, 1, MPI_INT, 
				dest, rank, &recvNum, 1, MPI_INT, 
				dest, dest, communicator, MPI_STATUS_IGNORE);	// DEST = partner Rank

	// Prepare New Buffers
	int keptLen = *len - sendNum;		// How long is array after sending?
	// Buffers
	recver = prep_buffs(recvNum);
	sender = prep_buffs(sendNum);
	kept_arr = prep_buffs(keptLen);

	// copy to send
	memcpy(sender, arr + startSendIdx, sizeof(double)*sendNum);
	// Copy whatever is not to be sent to other node
	if (startSendIdx > 0){
		memcpy(kept_arr, arr, sizeof(double)*keptLen);
	} else if (sendNum < *len) {
		memcpy(kept_arr, arr + sendNum, sizeof(double)*keptLen);
	}

	// Array length after Recving and sending
	int newSIze = keptLen + recvNum;
	if (newSIze > *len){
		arr = (double *)realloc(arr, newSIze * sizeof(double));
	}
	*len = newSIze;

	MPI_Sendrecv(sender, sendNum, MPI_DOUBLE, 
				dest, rank, recver, recvNum, MPI_DOUBLE, 
				dest, dest, communicator, MPI_STATUS_IGNORE);	// DEST = partner Rank
	
	memcpy(arr, kept_arr, sizeof(double)*keptLen);
	memcpy(arr + keptLen, recver, sizeof(double)*recvNum);

	//DEBUG
	// db_arr(arr, *len, "After SendRecv", rank, npPerGroup);
	return arr;
};

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
	int num_values;
	if (EOF == fscanf(file, "%d", &num_values)) {
		perror("Couldn't read element count from input file");
		printf("AT: %i", num_values);
		return -1;
	}
	if (NULL == (*values = malloc(num_values * sizeof(double)))) {
		perror("Couldn't allocate memory for input");
		return -1;
	}
	for (int i=0; i< num_values; i++) {
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
