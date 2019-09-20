using namespace std;

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>

typedef float DTYPE;

void readMatrix_simple(char *filename, DTYPE **h_in, int *rows, int *cols, int *nnz)
{
     string line;
     ifstream mfs( filename );
     
     if( !mfs.good() )
     {
          std::cerr << "Error: unable to open matrix file " << filename << std::endl;
          exit(1);
     }
     
     cout << "Reading header..." << endl;
     
     while (!getline( mfs, line ).eof() )
     {
        if (line[0] != '%') 
        {
            break; 
        }
     } 
	
     cout << "Reading matrix params..." << endl;
     sscanf(line.c_str(), "%d %d %d", rows, cols, nnz); 
     cout << "Matrix params are: " << *rows << ", " << *cols << ", " << *nnz << endl; 
          
     DTYPE *h_temp = (DTYPE *)malloc((*rows) * (*cols) * sizeof(DTYPE));
     
     cout << "Reading data..." << endl;
     
     int index = 0;
     while (!getline( mfs, line ).eof() )
     {
          sscanf(line.c_str(), "%f", &h_temp[index]);
          index++;
     }
     
     *h_in = h_temp;
     
     cout << "Finished reading input." << endl;
}

void transMatrix(DTYPE *h_in, int rows, int cols, DTYPE *h_trans)
{     
     // Finding transpose of matrix a[][] and storing it in array trans[][].
     for(int rowix = 0; rowix < rows; rowix++)
     {
          for(int colix = 0; colix < cols; colix++)
          {
               h_trans[colix*(rows)+rowix]=h_in[rowix*(cols)+colix];
          }
     }
     
     cout << "Finished transposing matrix." << endl;
}

// void readMatrix_matmark(char *filename, DTYPE **val_ptr, int **cols_ptr, int **rowDelimiters_ptr, int *n, int *size, int *columns) 
// {
    // std::string line;
    // char id[FIELD_LENGTH];
    // char object[FIELD_LENGTH]; 
    // char format[FIELD_LENGTH]; 
    // char field[FIELD_LENGTH]; 
    // char symmetry[FIELD_LENGTH]; 

    // std::ifstream mfs( filename );
    // if( !mfs.good() )
    // {
        // cerr << "Error: unable to open matrix file " << filename << endl;
        // exit( 1 );
    // }

    // int symmetric = 0; 
    // int pattern = 0; 

    // int nRows, nCols, nElements;  

    // struct Coordinate *coords;
 

    // // read matrix header
    // if( getline( mfs, line ).eof() )
    // {
        // std::cerr << "Error: file " << filename << " does not store a matrix" << std::endl;
        // exit( 1 );
    // }

    // sscanf(line.c_str(), "%s %s %s %s %s", id, object, format, field, symmetry); 

    // if (strcmp(object, "matrix") != 0) 
    // {
        // fprintf(stderr, "Error: file %s does not store a matrix\n", filename); 
        // exit(1); 
    // }
  
    // if (strcmp(format, "coordinate") != 0)
    // {
        // fprintf(stderr, "Error: matrix representation is dense\n"); 
        // exit(1); 
    // } 

    // if (strcmp(field, "pattern") == 0) 
    // {
        // pattern = 1; 
    // }

    // if (strcmp(symmetry, "symmetric") == 0) 
    // {
        // symmetric = 1; 
    // }

    // while (!getline( mfs, line ).eof() )
    // {
        // if (line[0] != '%') 
        // {
            // break; 
        // }
    // } 

	// cerr << "Reading data" << endl;

    // // read the matrix size and number of non-zero elements
    // sscanf(line.c_str(), "%d %d %d", &nRows, &nCols, &nElements); 
	// *columns=nCols;

    // int valSize = nElements;// * sizeof(struct Coordinate);
    // if (symmetric) 
    // {
        // valSize*=2; 
    // }               
	// cerr << "Setting number of elements: "<< valSize << endl;
    // coords = new Coordinate[valSize]; 

	// cerr << "Reading numbers from file" << endl;
    // int index = 0;
    // while (!getline( mfs, line ).eof() )
    // {
		// //cerr << index << " ";
        // if (pattern) 
        // {
            // sscanf(line.c_str(), "%d %d", &coords[index].x, &coords[index].y); 
            // // assign a random value 
            // coords[index].val = ((DTYPE) MAX_RANDOM_VAL * 
                                 // (rand() / (RAND_MAX + 1.0)));
        // }
        // else 
        // {
            // // read the value from file
            // sscanf(line.c_str(), "%d %d %f", &coords[index].x, &coords[index].y, 
                   // &coords[index].val); 
        // }

        // // convert into index-0-as-start representation
        // coords[index].x--;
        // coords[index].y--;    
        // index++; 

        // // add the mirror element if not on main diagonal
        // if (symmetric && coords[index-1].x != coords[index-1].y) 
        // {
            // coords[index].x = coords[index-1].y; 
            // coords[index].y = coords[index-1].x; 
            // coords[index].val = coords[index-1].val; 
            // index++;
        // }
    // }  

    // nElements = index; 
    // // sort the elements
    // qsort(coords, nElements, sizeof(struct Coordinate), coordcmp); 

	// cerr << "Allocating buffers for host data" << endl;

    // // create CSR data structures
    // *n = nElements; 
    // *size = nRows; 
    
    // *val_ptr =  (DTYPE *)sds_alloc(nElements * sizeof(DTYPE));
    // *cols_ptr =(int *)sds_alloc(nElements * sizeof(int));
    // *rowDelimiters_ptr = (int *)sds_alloc((nRows+1) * sizeof(int));
    
    // printf("nRows->%d nElements->%d\n",nRows,nElements);

    // DTYPE *val = *val_ptr; 
    // int *cols = *cols_ptr; 
    // int *rowDelimiters = *rowDelimiters_ptr;

    // rowDelimiters[0] = 0; 
    // rowDelimiters[nRows] = nElements; 
    // int r=0; 
    // for (int i=0; i<nElements; i++) 
    // {
        // while (coords[i].x != r) 
        // {
            // rowDelimiters[++r] = i; 
        // }
        // val[i] = coords[i].val; 
        // cols[i] = coords[i].y;
        
    // }
    
    // r = 0; 

    // delete[] coords;
// }

//Need only 3 size metrics since cols_h1 = rows_h2
void simplemm(DTYPE *h_in1, DTYPE *h_in2, int rows_h1, int rows_h2, int cols_h2, DTYPE *h_out)
{
     for (int i = 0; i < rows_h1; i++) 
     {
          for (int j = 0; j < cols_h2; j++)
          {    
               DTYPE temp = 0;
               for (int k = 0; k < rows_h2; k++)
               {
                    //temp += h_in1[i*cols_h1 + k] * h_in2[k*cols_h2 + i];
                    temp += h_in1[i*rows_h2 + k] * h_in2[k*cols_h2 + i];
               }
               h_out[i*cols_h2+j] = temp;
          }
     }
}

// //Matrix multiplication with a transposed h_in2
// void simplemm_trans(DTYPE *h_in1, DTYPE *h_in2, int rows_h1, int rows_h2, int cols_h2, DTYPE *h_out)
// {
     // for (int i = 0; i < rows_h1; i++) 
     // {
          // for (int j = 0; j < cols_h2; j++)
          // {    
               // DTYPE temp = 0;
               // for (int k = 0; k < rows_h2; k++)
               // {
                    // //temp += h_in1[i*cols_h1 + k] * h_in2[k*cols_h2 + i];
                    // temp += h_in1[i*rows_h2 + k] * h_in2[k*cols_h2 + i];
               // }
               // h_out[i*cols_h2+j] = temp;
          // }
     // }
// }

int main (int argc, char * argv[])
{

     if (argc < 3)
     {
          fprintf(stderr, "arguments: inFile1 inFile2 (matrixout)\n");
          exit(-1);
     }
          
     cout << "Reading matrix input files ..." << endl;
     
     DTYPE* h_in1;
     DTYPE* h_in2; 
     int rows_h1, rows_h2;
     int cols_h1, cols_h2;
     int nnz_h1, nnz_h2;
     
     readMatrix_simple(argv[1], &h_in1, &rows_h1, &cols_h1, &nnz_h1);
     cout << "Matrix params are: " << rows_h1 << ", " << cols_h1 << ", " << nnz_h1 << endl; 
     if ((!rows_h1) || (!cols_h1) || (!nnz_h1)) {
          fprintf(stderr,"Malloc failed (input matrix 1)\n");
          exit(1);
     }
     else
     {
          fprintf(stderr,"Malloc sucess (input matrix 1)\n");	
     }
          
     readMatrix_simple(argv[2], &h_in2, &rows_h2, &cols_h2, &nnz_h2);
     if ((!rows_h2) || (!cols_h2) || (!nnz_h2)) {
          fprintf(stderr,"Malloc failed (input matrix 2)\n");
          exit(1);
     }
     else
     {
          fprintf(stderr,"Malloc sucess (input matrix 2)\n");	
     }

     if (cols_h1 != rows_h2)
     {
          fprintf(stderr,"Matrix size missmatch. Unable to multiply!\n");
          exit(1);
     }
     
     DTYPE* h_trans = (DTYPE *)malloc((rows_h2) * (cols_h2) * sizeof(DTYPE));
     
     transMatrix(h_in2, rows_h2, cols_h2, h_trans);
     
     DTYPE* h_out = (DTYPE *)malloc(rows_h1 * cols_h2 * sizeof(DTYPE));     
     
     
     // cout << "Performing MM ..." << endl;
     // simplemm(h_in1,h_in2,rows_h1,rows_h2,cols_h2,h_out);
     // cout << "Done!" << endl;
     
     if (argv[3]) {
          ofstream out(argv[3]);
          cout << "Generating output ..." << endl;          
          // for (int i=0; i<rows_h1; i++) {
               // for(int j = 0; j < cols_h2; j++) {                
                    // out << h_out[i*cols_h2+j] << "\n"; 
               // }
          // }
          for (int i=0; i<rows_h2; i++) {
               for(int j = 0; j < cols_h2; j++) {                
                    //out << h_trans[j*rows_h2+i] << "\n"; 
                    out << h_trans[i*cols_h2+j] << "\n";
               }
          }
          out.close();
          cout << "Done!" << endl;
     }    
     

}     