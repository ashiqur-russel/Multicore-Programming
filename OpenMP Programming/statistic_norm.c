#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <omp.h>

/*********************************************************************************************
 * The following functions will be used for the validation of your program. These are the
 * functions that you should modify. You are allowed to define new functions that you call
 * from these functions. The functions you can modify:
 *     - int statistic_norm_matrix(double*, double*, int)
 *     - void matrix_multiplication(double*, double*, double*, int)
 *
 * The function test_results can be used to define your own test cases if you want.
 * However, the function is not used for the validation and will be overwritten by OpenSubmit.
 *
 * All other functions are overwritten by OpenSubmit which removes any changes!!!
 * Namely the overwritten functions are:
 *     - void main(int, char**)
 *     - int read_doubles(char*, double*);
 *     - int read_matrix(char*, double*);
 *     - int random_matrix(char*, double*, int);
 *     - void print_matrix(char*, double*, int);
 *
 * To compile this function add the  -lm  flag to the compiler.
 *
 *
 * Please fill in your personal information here:
 *
 * Name: Mohammad Ashiqur Rahman
 * MatNr: 558754
 ********************************************************************************************/

/*********************************************************************************************
 * Function declarations overwritten by OpenSubmit.
 ********************************************************************************************/

int random_matrix(char *matrixname, double *matrix, int matrix_size);
void print_matrix(char *matrixname, double *matrix, int matrix_size);

/*********************************************************************************************
 * Function declarations which calculate the statistic normalized matrix.
 * These are the functions where your parallelization should be inserted.
 ********************************************************************************************/

/***
 * calculates the normalization factors for the columns of a matrix and stores them into the diagonal of norm_matrix
 *
 * @param source_matrix  the matrix for which the column normalization factor should be computed
 * @param norm_matrix    the matrix getting filled with the norm factors on the diagonal elements
 * @param size           the size of the matrices (i.e. number of rows == number of columns)
 * @returns              -1 in case of an error; 0 otherwise
 ***/

#define NUM_TRHEADS 4

int max(int a, int b)
{
    return a >= b ? a : b;
}

int statistic_norm_matrix(double *source_matrix, double *norm_matrix, int size)
{
    int flag = 1;

    omp_set_num_threads(NUM_TRHEADS);

#pragma omp parallel
    {
        int row;
        int id, n_threads, col;
        id = omp_get_thread_num();
        n_threads = omp_get_num_threads();

        for (col = id; col < size; col += n_threads)
        { //Assumming NUM_THREADS = 2, Thread 0 will handle column 0, 2, 4, 6.... Thread 1 will handle column 1,3,5,7....
            if (flag == -1)
                continue;

            norm_matrix[col * size + col] = source_matrix[col];

            for (row = 0; row < size; row++)
            {
                norm_matrix[col * size + col] = max(norm_matrix[col * size + col], source_matrix[row * size + col]);
            }

            if (norm_matrix[col * size + col] == 0)
            {
                flag = -1;
                continue;
            }

            norm_matrix[col * size + col] = (1 / norm_matrix[col * size + col]);
        }
    }

    if (flag == -1)
    {
        printf("can't process a matrix where the max col value is 0");
        return flag;
    }

    print_matrix("matrix-norm", norm_matrix, size);
    return 0;
}

/***
 * mulitplies two matrices with each other, these are the source_matrix and norm_matrix
 *
 * @param source_matrix  the source matrix which should be normalized
 * @param norm_matrix    the matrix containing the norm factors on the diagonal elements
 * @param dest_matrix    the matrix containg the normalized columns
 * @param size           the size of the matrices (i.e. number of rows == number of columns)
 ***/
void matrix_multiplication(double *source_matrix, double *norm_matrix, double *dest_matrix, int size)
{
    omp_set_num_threads(NUM_TRHEADS);

#pragma omp parallel
    {
        int row, col, k;
        int id, n_threads;
        id = omp_get_thread_num();
        n_threads = omp_get_num_threads();

        for (row = id; row < size; row += n_threads)
        {
            for (col = 0; col < size; col++)
            {
                dest_matrix[row * size + col] = 0;
                for (k = 0; k < size; k++)
                {
                    dest_matrix[row * size + col] += source_matrix[row * size + k] * norm_matrix[k * size + col];
                }
            }
        }
    }
}

/*********************************************************************************************
 * Your changes in the main function will be overwritten by OpenSubmit!!!
 * Your parallelization should be made inside the statistic_norm_matrix and
 * matrix_multiplication function.
 ********************************************************************************************/

int main(int argc, char *argv[])
{
    int max_matrix_size = 100;
    double *matrix = malloc(max_matrix_size * max_matrix_size * sizeof(double));
    double *norm_matrix;
    double *dest_matrix;

    int matrix_size = random_matrix("matrix-input", matrix, max_matrix_size);

    if (matrix_size < 0)
    {
        printf("error while reading matrix");
        free(matrix);
        return 1;
    }

    norm_matrix = malloc(matrix_size * matrix_size * sizeof(double));
    memset(norm_matrix, 0, matrix_size * matrix_size * sizeof(double));
    dest_matrix = malloc(matrix_size * matrix_size * sizeof(double));

    double start = omp_get_wtime();
    if (statistic_norm_matrix(matrix, norm_matrix, matrix_size) < 0)
    {
        printf("error while calculating the norm matrix");
        free(matrix);
        free(norm_matrix);
        return 1;
    }
    printf("Finished matrix norm: ");
    double mid = omp_get_wtime();
    printf("Time taken: %f seconds\n", mid - start);

    matrix_multiplication(matrix, norm_matrix, dest_matrix, matrix_size);
    printf("Finished matrix Mult: ");
    double end = omp_get_wtime();
    printf("Time taken: %f seconds\n", end - mid);

    print_matrix("matrix-output", dest_matrix, matrix_size);

    free(matrix);
    free(norm_matrix);
    free(dest_matrix);
    return 0;
}

/*********************************************************************************************
 * The remainder of this file consists of helper functions.
 * These functions should not be changed
 * All changes in these functions will be overwritten by OpenSubmit!!!
 ********************************************************************************************/

/***
 * initializes a matrix with random values and writes matrix to matrixname
 * the max size of one dimension is defined by #define max_matrix_size 1000
 *
 * @param matrixname    matrixname for the output file
 * @param matrix        pointer to a matrix
 * @param matrix_size   amount of items for one dimension
 * @return              matrix size for one dimension
 ***/
int random_matrix(char *matrixname, double *matrix, int matrix_size)
{
    int i, j;
    double r;
    srand(time(NULL));

    for (i = 0; i < matrix_size; i++)
    {
        for (j = 0; j < matrix_size; j++)
        {
            r = (double)(rand() % 10000);
            r /= 100;
            matrix[i * matrix_size + j] = r;
        }
    }

    //print_matrix(matrixname, matrix, matrix_size);
    return matrix_size;
}

/***
 * write given matrix to the provided matrixname or stdout
 * the behaviour is influenced by #define print_instead_of_write
 *
 * @param matrixname    matrixname for the output file
 * @param matrix        pointer to a matrix
 * @param matrix_size   amount of items for one dimension
 ***/
void print_matrix(char *matrixname, double *matrix, int matrix_size)
{
    int i, j;

    FILE *fp = stdout;
    fprintf(fp, "Name: %s\n", matrixname);

    for (i = 0; i < matrix_size; i++)
    {
        for (j = 0; j < matrix_size; j++)
        {
            fprintf(fp, "%f;", matrix[i * matrix_size + j]);
        }
        fprintf(fp, "\n");
    }
    fprintf(fp, "\n");
}