#ifndef QSORTHELP
#define QSORTHELP

#include <string.h>

int read_input(const char *file_name, double **values);
int write_output(char *file_name, const double *output, int num_values);
int compare(const void* num1, const void* num2);
void sort(double *arr, int *len, int phase, MPI_Comm communicator);
void exchange_Numbers(double *arr, int *len, int rank, int phase, double *Narr);
double * prep_buffs(int newSize);
void db_arr(double *ptr, int len, char *msg, int rank, int npPerGroup);
double median(double *arr, int len);

int num_elems_per_processor = 0;

#endif // QSORTHELP