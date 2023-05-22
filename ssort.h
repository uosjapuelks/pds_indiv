#ifndef QSORTHELP
#define QSORTHELP

#include <string.h>

int read_input(const char *file_name, double **values);
int write_output(char *file_name, const double *output, int num_values);
int compare(const void* num1, const void* num2);
void sort(double *arr, int *len, int phase, double *output, double *Narr);
void exchange_Numbers(double *arr, int *len, int rank, int phase, double *Narr);
void gather_Numbers(double *arr_in, int len, double *output, MPI_Comm communicator);
double * prep_buffs(int newSize);
void db_arr(double *ptr, int len, char *msg, int rank, int npPerGroup);
void flip_isOdd();
int num_rows_per_processor = 0;
int num_elems_recv = 0;
int *atav_scount, *atav_sdispls;
MPI_Datatype column_type;
MPI_Datatype column_resized;
int *gfinLens;
int *gdispls;
int num_values;
int isOdd;
int startIndicator; // Starting row is odd or even (0 is even, 1 is odd)
int num_rows_recv;
double *tmp;
double *Narr;
double *sort_tmp;
int rank, num_processors, N;
int stepsNeeded;

#endif // QSORTHELP