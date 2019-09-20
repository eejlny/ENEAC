//============================================================================
// Name			: SPMV.h
// Author		     : Antonio Vilches
// Version		: 1.0
// Date			: 13 / 01 / 2015
// Copyright	     : Department. Computer's Architecture (c)
// Description	     : Main file of SPMV 
//============================================================================


#ifndef SPMM_UTIL_H_
#define SPMM_UTIL_H_

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string.h>
#include <math.h>

#include  "kernelspmm.h"

using namespace std;

/*****************************************************************************
 * Global Variables 
 * **************************************************************************/
int numRows;
int numColumns;
int numNonZeroes;
int nItems;
float * h_val;
int * h_cols; 
int * h_rowDelimiters;
float * h_vec;
float * h_out;
float * h_vec_trans;
float * h_out_trans;
//Functions and variables for noncache memory buffers for use with HP connected accelerators
// //#define HP //choose either HP or HPC pragma
// #define HPC
float * h_val_noncache;
int * h_cols_noncache; 
int * h_rowDelimiters_noncache;
float * h_vec_noncache_trans;
float * h_out_noncache_trans;
     
int nItemsPadded;
float maxval;
int rowsPerThread;
int step;

const static int ROW_SIZE_MAX = (30000);
const static int COL_SIZE_MAX = ROW_SIZE_MAX;
const int x_width = 100; //if you change this then both CPUs and FPGA will process this number of x vectors

const int debug_flag = 0; //enable debug information

/*****************************************************************************
 * Constants
 * **************************************************************************/

// threshold for error in GPU results
static const double MAX_RELATIVE_ERROR = .02;

// alignment factor in terms of number of floats, used to enforce
// memory coalescing 
static const int PAD_FACTOR = 16;

// size of atts buffer
static const int TEMP_BUFFER_SIZE = 1024;    

// length of array for reading fields of mtx header
static const int FIELD_LENGTH = 128;

// If using a matrix market pattern, assign values from 0-MAX_RANDOM_VAL
static const float MAX_RANDOM_VAL = 10.0f;

struct Coordinate {
    int x; 
    int y; 
    float val; 
};

/*****************************************************************************
 * Functions
 * **************************************************************************/

bool verifyResults(const float *cpuResults, const float *gpuResults, 
                   const int size, const int pass = -1) {

    bool passed = true; 
    for (int i=0; i<size; i++) 
    {
        if (fabs(cpuResults[i] - gpuResults[i]) / cpuResults[i] 
            > MAX_RELATIVE_ERROR) 
        {
#ifdef VERBOSE_OUTPUT
           cout << "Mismatch at i: "<< i << " ref: " << cpuResults[i] << 
                " dev: " << gpuResults[i] << endl;
#endif
            passed = false; 
        }
    }

    if (pass != -1) 
    {
        cout << "Pass "<<pass<<": ";
    }
    if (passed) 
    {
        cout << "Passed" << endl;
    }
    else 
    {
        cout << "---FAILED---" << endl;
    }
    return passed;
}

template <typename DTYPE>
void spmvCpu(const DTYPE *val, const int *cols, const int *rowDelimiters, const DTYPE *vec, DTYPE *out, int num_rows, unsigned int begin, unsigned int end) 
{
     //cout << "<CPU> ROWS --- begin: " << begin << "; end: " << end << "\n";
     for(int xw = 0; xw < x_width; xw++)
     {
          for (int i = begin; i < end; i++) 
          {
			DTYPE o_temp = 0;
			for (int j=rowDelimiters[i]; j < rowDelimiters[i+1]; j++)
			{
                    int col = cols[j];
				o_temp += val[j] * vec[col*x_width+i];
			}
			out[i*x_width+xw] = o_temp;
		}
	}
}

template <typename DTYPE>
void spmv_test(
int *rowSize_local_rs,
int *rowSize_local_nrs,
int *columnIndex,
DTYPE *values,
DTYPE *y,
DTYPE *x_local,
int row_size,
int nnz,
int new_nnz)
{
	int row_size_tmp=0;
	int j = 0;

	DTYPE y_tmp = 0;
	int row_counter = 0;

	int row_size_remains = 0;
	for (int i = 0; i < new_nnz; i+=1) {
		if (row_size_tmp == 0) {
			row_size_tmp = rowSize_local_nrs[j];
			row_size_remains = 0;
			y_tmp = 0;
			row_counter	= rowSize_local_rs[j++];
		}
          
          DTYPE y_local = 0;
		for (int p = 0; p < 1; p++) {
			row_size_remains++;
			if (row_size_remains > row_counter) {
				y_local +=  0;
			} else {
				DTYPE v = values[i];
				int   ci = columnIndex[i];
                    y_local +=  v*x_local[ci];
			}
		}

		y_tmp += y_local;
		row_size_tmp-=1;

		if (row_size_tmp == 0) {
               y[j-1] = y_tmp;
		}
	}
}

template <typename DTYPE>
void spmm_fast_test(
int *rowPtr,
int *columnIndex,
DTYPE *values,
DTYPE *y,
DTYPE *x,
int col_size,
int row_size,
int nnz,
int & error_condition) 
{
     //detect if buffer sizes are being exceeded
     if ((col_size > COL_SIZE_MAX) || (row_size > ROW_SIZE_MAX))
          error_condition=1;
     else
          error_condition=0;
          
     if(error_condition == 0) {
          int rowSizeNew_local_rs[ROW_SIZE_MAX];
          int rowSizeNew_local_nrs[ROW_SIZE_MAX];
          DTYPE x_local[COL_SIZE_MAX];
          DTYPE y_local[ROW_SIZE_MAX];

          int new_nnz = nnz;
          int idx_previous = rowPtr[0];
          for (int i=1; i<(row_size+1); i++) {
               int rs;
               int nrs;
               int idx2 = rowPtr[i];
               rs = idx2 - idx_previous;
               idx_previous = idx2;
               if (rs  == 0) {
                    nrs = 1;
                    new_nnz += 1;
               }
               else if (rs%1 == 0) {
                    nrs = rs;
                    new_nnz += 0;
               } else {
                    nrs = rs + (1-rs%1);
                    new_nnz += (1-rs%1);
               }
               rowSizeNew_local_rs[i-1] = rs;
               rowSizeNew_local_nrs[i-1] = nrs;
          }
          
          for (int x_index = 0; x_index < x_width; x_index++) {
               for (int i=0; i<(col_size); i++)   {
                     x_local[i] = x[i*x_width+x_index];
               }
               
               spmv_test(rowSizeNew_local_rs, rowSizeNew_local_nrs, columnIndex, values, y_local, x_local, row_size, nnz, new_nnz);
               
               for (int i=0; i<(row_size); i++){
                     y[i*x_width + x_index] = y_local[i];
               }              
          }
     }
}

template <typename DTYPE>
void spmvCpu_fpgacode(
DTYPE *values,
int *columnIndex,
int *rowPtr,
DTYPE *x, 
DTYPE *y, 
int row_size,
unsigned int begin,
unsigned int end)
{
     int ret_value;
	int line_count = end-begin;
	int nnz_int, nnz_done;
	int *rowPtr_int;
	int *columnIndex_int;
	int error_condition;

	DTYPE *values_int;
	DTYPE *y_int;
	
     rowPtr_int = rowPtr + begin;
	nnz_done = rowPtr_int[0];
	columnIndex_int = columnIndex + nnz_done;
	values_int = values + nnz_done;
	nnz_int = rowPtr[end] - rowPtr[begin];
	y_int = y + begin*x_width;

	error_condition = 0;

	spmm_fast_test(rowPtr_int, columnIndex_int, values_int, y_int, x, row_size, line_count, nnz_int, error_condition);

	if (error_condition)
	{
		fprintf(stderr,"Error: FPGA buffer memory overflow\n");
		exit(1);
	}
}

inline int intcmp(const void *v1, const void *v2);
inline int coordcmp(const void *v1, const void *v2);
template <typename DTYPE>
void readMatrix(char *filename, DTYPE **val_ptr, int **cols_ptr, 
                int **rowDelimiters_ptr, int *n, int *size);
template <typename DTYPE>
void fill(DTYPE *A, const int n, const float maxi);
void initRandomMatrix(int *cols, int *rowDelimiters, const int n, const int dim);
template <typename DTYPE>
void printSparse(DTYPE *A, int n, int dim, int *cols, int *rowDelimiters);
template <typename DTYPE>
void convertToColMajor(DTYPE *A, int *cols, int dim, int *rowDelimiters, 
                       DTYPE *newA, int *newcols, int *rl, int maxrl, 
                       bool padded);
template <typename DTYPE>
void convertToPadded(DTYPE *A, int *cols, int dim, int *rowDelimiters, 
                     DTYPE **newA_ptr, int **newcols_ptr, int *newIndices, 
                     int *newSize); 

template <typename DTYPE>
void convertToPadded_noncache(DTYPE *A, int *cols, int dim, int *rowDelimiters, 
                     DTYPE **newA_ptr, int **newcols_ptr, int *newIndices, 
                     int *newSize); 

// ****************************************************************************
// Function: readMatrix
//
// Purpose:
//   Reads a sparse matrix from a file of Matrix Market format 
//   Returns the data structures for the CSR format
//
// Arguments:
//   filename: c string with the name of the file to be opened
//   val_ptr: input - pointer to uninitialized pointer
//            output - pointer to array holding the non-zero values
//                     for the  matrix 
//   cols_ptr: input - pointer to uninitialized pointer
//             output - pointer to array of column indices for each
//                      element of the sparse matrix
//   rowDelimiters: input - pointer to uninitialized pointer
//                  output - pointer to array holding
//                           indices to rows of the matrix
//   n: input - pointer to uninitialized int
//      output - pointer to an int holding the number of non-zero
//               elements in the matrix
//   size: input - pointer to uninitialized int
//         output - pointer to an int holding the number of rows in
//                  the matrix 
//
// Programmer: Lukasz Wesolowski
// Creation: July 2, 2010
// Returns:  nothing directly
//           allocates and returns *val_ptr, *cols_ptr, and
//           *rowDelimiters_ptr indirectly 
//           returns n and size indirectly through pointers
// ****************************************************************************
template <typename DTYPE>
void readMatrix(char *filename, DTYPE **val_ptr, int **cols_ptr, 
                int **rowDelimiters_ptr, int *n, int *size, int *columns) 
{
    std::string line;
    char id[FIELD_LENGTH];
    char object[FIELD_LENGTH]; 
    char format[FIELD_LENGTH]; 
    char field[FIELD_LENGTH]; 
    char symmetry[FIELD_LENGTH]; 

    std::ifstream mfs( filename );
    if( !mfs.good() )
    {
        std::cerr << "Error: unable to open matrix file " << filename << std::endl;
        exit( 1 );
    }

    int symmetric = 0; 
    int pattern = 0; 

    int nRows, nCols, nElements;  

    struct Coordinate *coords;

	cerr << "Reading header" << endl;

    // read matrix header
    if( getline( mfs, line ).eof() )
    {
        std::cerr << "Error: file " << filename << " does not store a matrix" << std::endl;
        exit( 1 );
    }

    sscanf(line.c_str(), "%s %s %s %s %s", id, object, format, field, symmetry); 

    if (strcmp(object, "matrix") != 0) 
    {
        fprintf(stderr, "Error: file %s does not store a matrix\n", filename); 
        exit(1); 
    }
  
    if (strcmp(format, "coordinate") != 0)
    {
        fprintf(stderr, "Error: matrix representation is dense\n"); 
        exit(1); 
    } 

    if (strcmp(field, "pattern") == 0) 
    {
        pattern = 1; 
    }

    if (strcmp(symmetry, "symmetric") == 0) 
    {
        symmetric = 1; 
    }

    while (!getline( mfs, line ).eof() )
    {
        if (line[0] != '%') 
        {
            break; 
        }
    } 

	cerr << "Reading data" << endl;

    // read the matrix size and number of non-zero elements
    sscanf(line.c_str(), "%d %d %d", &nRows, &nCols, &nElements); 
	*columns=nCols;

    int valSize = nElements;// * sizeof(struct Coordinate);
    if (symmetric) 
    {
        valSize*=2; 
    }               
	cerr << "Setting number of elements: "<< valSize << endl;
    coords = new Coordinate[valSize]; 

	cerr << "Reading numbers from file" << endl;
    int index = 0;
    while (!getline( mfs, line ).eof() )
    {
		//cerr << index << " ";
        if (pattern) 
        {
            sscanf(line.c_str(), "%d %d", &coords[index].x, &coords[index].y); 
            // assign a random value 
            coords[index].val = ((DTYPE) MAX_RANDOM_VAL * 
                                 (rand() / (RAND_MAX + 1.0)));
        }
        else 
        {
            // read the value from file
            sscanf(line.c_str(), "%d %d %f", &coords[index].x, &coords[index].y, 
                   &coords[index].val); 
        }

        // convert into index-0-as-start representation
        coords[index].x--;
        coords[index].y--;    
        index++; 

        // add the mirror element if not on main diagonal
        if (symmetric && coords[index-1].x != coords[index-1].y) 
        {
            coords[index].x = coords[index-1].y; 
            coords[index].y = coords[index-1].x; 
            coords[index].val = coords[index-1].val; 
            index++;
        }
    }  

    nElements = index; 
    // sort the elements
    qsort(coords, nElements, sizeof(struct Coordinate), coordcmp); 

	cerr << "Allocating buffers for host data" << endl;

    // create CSR data structures
    *n = nElements; 
    *size = nRows; 
    
    *val_ptr =  (DTYPE *)sds_alloc(nElements * sizeof(DTYPE));
    *cols_ptr =(int *)sds_alloc(nElements * sizeof(int));
    *rowDelimiters_ptr = (int *)sds_alloc((nRows+1) * sizeof(int));
    
    printf("nRows->%d nElements->%d\n",nRows,nElements);

    DTYPE *val = *val_ptr; 
    int *cols = *cols_ptr; 
    int *rowDelimiters = *rowDelimiters_ptr;

    rowDelimiters[0] = 0; 
    rowDelimiters[nRows] = nElements; 
    int r=0; 
    for (int i=0; i<nElements; i++) 
    {
        while (coords[i].x != r) 
        {
            rowDelimiters[++r] = i; 
        }
        val[i] = coords[i].val; 
        cols[i] = coords[i].y;
        
    }
    
    r = 0; 

    delete[] coords;
}    

// ****************************************************************************
// Function: fill
//
// Purpose:
//   Simple routine to initialize input array
//
// Arguments:
//   A: pointer to the array to initialize
//   n: number of elements in the array
//   maxi: specifies range of random values
//
// Programmer: Lukasz Wesolowski
// Creation: June 21, 2010
// Returns:  nothing
//
// ****************************************************************************
template <typename DTYPE>
void fill(DTYPE *A, const int n, const float maxi)
{
    for (int j = 0; j < n; j++) 
    {
        A[j] = ((DTYPE) maxi * (rand() / (RAND_MAX + 1.0f)));
    }
}

// ****************************************************************************
// Function initRandomMatrix
//
// Purpose:
//   Assigns random positions to a given number of elements in a square
//   matrix, A.  The function encodes these positions in compressed sparse
//   row format.
//
// Arguments:
//   cols:          array for column indexes of elements (size should be = n)
//   rowDelimiters: array of size dim+1 holding indices to rows of A;
//                  last element is the index one past the last element of A
//   n:             number of nonzero elements in A
//   dim:           number of rows/columns in A
//
// Programmer: Kyle Spafford
// Creation: July 28, 2010
// Returns: nothing
//
// ****************************************************************************
void initRandomMatrix(int *cols, int *rowDelimiters, const int n, const int dim)
{
    int nnzAssigned = 0;

    // Figure out the probability that a nonzero should be assigned to a given
    // spot in the matrix
    double prob = (double)n / ((double)dim * (double)dim);

    // Seed random number generator
    srand(8675309L);

    // Randomly decide whether entry i,j gets a value, but ensure n values
    // are assigned
    bool fillRemaining = false;
    for (int i = 0; i < dim; i++)
    {
        rowDelimiters[i] = nnzAssigned;
        for (int j = 0; j < dim; j++)
        {
            int numEntriesLeft = (dim * dim) - ((i * dim) + j);
            int needToAssign   = n - nnzAssigned;
            if (numEntriesLeft <= needToAssign) {
                fillRemaining = true;
            }
            if ((nnzAssigned < n && rand() <= prob) || fillRemaining)
            {
                // Assign (i,j) a value
                cols[nnzAssigned] = j;
                nnzAssigned++;
            }
        }
    }
    // Observe the convention to put the number of non zeroes at the end of the
    // row delimiters array
    rowDelimiters[dim] = n;
    assert(nnzAssigned == n);
}

// ****************************************************************************
// Function printSparse
//
// Purpose:
//   Prints a sparse matrix in dense form for debugging purposes
//
// Arguments:
//   A: array holding the non-zero values for the matrix
//   n: number of elements in A
//   dim: number of rows/columns in the matrix
//   rowDelimiters: array of size dim+1 holding indices to rows of A; 
//               last element is the index one past the last element of A
// 
// Programmer: Lukasz Wesolowski
// Creation: June 22, 2010
// Returns: nothing
//
// ****************************************************************************
template <typename DTYPE>
void printSparse(DTYPE *A, int n, int dim, int *cols, int *rowDelimiters) 
{

    int colIndex; 
    int zero = 0; 

    for (int i=0; i<dim; i++) 
    {
        colIndex = 0; 
        for (int j=rowDelimiters[i]; j<rowDelimiters[i+1]; j++) 
        {
            while (colIndex++ < cols[j]) 
            {
                printf("%7d ", zero); 
            }
            printf("%1.1e ", A[j]);; 
        }
        while (colIndex++ < dim) 
        {
            printf("%7d ", zero);
        }
        printf("\n"); 
    }

}

// ****************************************************************************
// Function: convertToColMajor
//
// Purpose: 
//   Converts a sparse matrix representation whose data structures are
//   in row-major format into column-major format. 
//
// Arguments: 
//   A: array holding the non-zero values for the matrix in 
//      row-major format
//   cols: array of column indices of the sparse matrix in 
//         row-major format
//   dim: number of rows/columns in the matrix
//   rowDelimiters: array holding indices in A to rows of the sparse matrix 
//   newA: input - buffer of size dim * maxrl
//         output - A in ELLPACK-R format
//   newcols: input - buffer of same size as newA
//            output - cols in ELLPACK-R format
//   rl: array storing length of every row of A
//   maxrl: maximum number of non-zero elements per row in A
//   padded: indicates whether newA should be padded so that the
//           number of rows divides PAD_FACTOR
//
// Programmer: Lukasz Wesolowski
// Creation: June 29, 2010
// Returns:
//   nothing directly
//   newA and newcols indirectly through pointers
// ****************************************************************************
template <typename DTYPE>
void convertToColMajor(DTYPE *A, int *cols, int dim, int *rowDelimiters, 
                       DTYPE *newA, int *newcols, int *rl, int maxrl, 
                       bool padded) 
{
    int pad = 0; 
    if (padded && dim % PAD_FACTOR != 0) 
    {
        pad = PAD_FACTOR - dim % PAD_FACTOR; 
    }

    int newIndex = 0; 
    for (int j=0; j<maxrl; j++) 
    {
        for (int i=0; i<dim; i++) 
        {
            if (rowDelimiters[i] + j < rowDelimiters[i+1]) 
            { 
                newA[newIndex] = A[rowDelimiters[i]+j]; 
                newcols[newIndex] = cols[rowDelimiters[i]+j]; 
            }
            else 
            {
                newA[newIndex] = 0; 
            }
            newIndex++; 
        }
        if (padded) 
        {
            for (int p=0; p<pad; p++) 
            {
                newA[newIndex] = 0; 
                newIndex++;
            }
        }
    }
}

// ****************************************************************************
// Function: convertToPadded
//
// Purpose: pads a CSR matrix with zeros so that each line of values
//          for the matrix is aligned to PAD_FACTOR*4 bytes
//
// Arguments: 
//   A: array holding the non-zero values for the matrix 
//   cols: array of column indices of the sparse matrix 
//   dim: number of rows/columns in the matrix
//   rowDelimiters: array holding indices in A to rows of the sparse matrix 
//   newA_ptr: input - pointer to an uninitialized pointer
//             output - pointer to padded A
//   newcols_ptr: input - pointer to an uninitialized pointer
//                output - pointer to padded cols
//   newIndices: input - buffer of size dim + 1 
//               output - array holding indices in newA to rows of the
//                        sparse matrix
//   newSize: input - pointer to uninitialized int
//            output - pointer to the size of A
//
// Programmer: Lukasz Wesolowski
// Creation: July 8, 2010
// Returns:
//   nothing directly
//   allocates and returns *newA_ptr and *newcols_ptr indirectly 
//   returns newIndices and newSize indirectly through pointers
// ****************************************************************************
template <typename DTYPE>
void convertToPadded(DTYPE *A, int *cols, int dim, int *rowDelimiters, 
                     DTYPE **newA_ptr, int **newcols_ptr, int *newIndices, 
                     int *newSize) 
{

    // determine total padded size and new row indices
    int paddedSize = 0;  
    int rowSize; 

    for (int i=0; i<dim; i++) 
    {    
        newIndices[i] = paddedSize; 
        rowSize = rowDelimiters[i+1] - rowDelimiters[i]; 
        if (rowSize % PAD_FACTOR != 0) 
        {
            rowSize += PAD_FACTOR - rowSize % PAD_FACTOR; 
        } 
        paddedSize += rowSize; 
    }
    *newSize = paddedSize; 
    newIndices[dim] = paddedSize; 
    
    *newA_ptr =  (DTYPE *)sds_alloc(paddedSize * sizeof(DTYPE));
    *newcols_ptr =  (DTYPE *)sds_alloc(paddedSize * sizeof(int));

     printf("padded not supported\n");
     exit(1);		
   

  
    DTYPE *newA = *newA_ptr; 
    int *newcols = *newcols_ptr; 

    memset(newA, 0, paddedSize * sizeof(DTYPE)); 
    memset(newcols, 0, paddedSize*sizeof(int));

    // fill newA and newcols
    for (int i=0; i<dim; i++) 
    {
        for (int j=rowDelimiters[i], k=newIndices[i]; j<rowDelimiters[i+1]; 
             j++, k++) 
        {
            newA[k] = A[j]; 
            newcols[k] = cols[j]; 
        }
    }

}

template <typename DTYPE>
void convertToPadded_noncache(DTYPE *A, int *cols, int dim, int *rowDelimiters, 
                     DTYPE **newA_ptr, int **newcols_ptr, int *newIndices, 
                     int *newSize) 
{

    // determine total padded size and new row indices
    int paddedSize = 0;  
    int rowSize; 

    for (int i=0; i<dim; i++) 
    {    
        newIndices[i] = paddedSize; 
        rowSize = rowDelimiters[i+1] - rowDelimiters[i]; 
        if (rowSize % PAD_FACTOR != 0) 
        {
            rowSize += PAD_FACTOR - rowSize % PAD_FACTOR; 
        } 
        paddedSize += rowSize; 
    }
    *newSize = paddedSize; 
    newIndices[dim] = paddedSize; 

    *newA_ptr =  (DTYPE *)sds_alloc_non_cacheable(paddedSize * sizeof(DTYPE));
    *newcols_ptr =  (DTYPE *)sds_alloc_non_cacheable(paddedSize * sizeof(int));

     printf("padded not supported\n");
     exit(1);		
   

  
    DTYPE *newA = *newA_ptr; 
    int *newcols = *newcols_ptr; 

    memset(newA, 0, paddedSize * sizeof(DTYPE)); 
    memset(newcols, 0, paddedSize*sizeof(int));

    // fill newA and newcols
    for (int i=0; i<dim; i++) 
    {
        for (int j=rowDelimiters[i], k=newIndices[i]; j<rowDelimiters[i+1]; 
             j++, k++) 
        {
            newA[k] = A[j]; 
            newcols[k] = cols[j]; 
        }
    }

}

// comparison functions used for qsort

inline int intcmp(const void *v1, const void *v2)
{
    return (*(int *)v1 - *(int *)v2);
}


inline int coordcmp(const void *v1, const void *v2)
{
    struct Coordinate *c1 = (struct Coordinate *) v1;
    struct Coordinate *c2 = (struct Coordinate *) v2; 

    if (c1->x != c2->x) 
    {
        return (c1->x - c2->x); 
    }
    else 
    {
        return (c1->y - c2->y); 
    }
}

#endif // SPMV_UTIL_H_
