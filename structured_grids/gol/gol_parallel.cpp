#include "gol_parallel.h"
#include <cstdlib>



// Computes the given number of iterations for an n x m array of cells using
// the given number of threads.
void gol_parallel(bool* cells, int n, int m, int iterations, int nthreads) {

	 int t, i, j;
    int neighbors;
    //bool* cells_new = malloc(n * m * sizeof(bool));
	bool* cells_new = static_cast<bool*>(malloc(n * m * sizeof(bool)));

void gol_parallel(bool* cells, int n, int m, int iterations, int nthreads)
{
    bool* cells_new = static_cast<bool*>(malloc(n * m * sizeof(bool)));


    #pragma acc data copy(cells[0:n*m]) create(cells_new[0:n*m])
    {
        for (int t = 0; t < iterations; t++) {


    for(t = 0; t < iterations; ++t) {
        #pragma omp parallel for num_threads(nthreads) private(j,neighbors) shared (cells, cells_new, n,m) schedule(runtime)
        for(i = 0; i < n; ++i) {
            for(j = 0; j < m; ++j) {

	        neighbors = 0;
                if(i>0 && j>0){
			neighbors += cells[(i-1) * m + j-1] ? 1 : 0;
			if(i>1 && j>1)
				neighbors += cells[(i-2) * m + j-2] ? 1 : 0;
		}
                if(i>0 && j<m-1){
			neighbors += cells[(i-1) * m + j+1] ? 1 : 0;
			if(i>1 && j<m-2)
				neighbors += cells[(i-2) * m + j+2] ? 1 : 0;
		}
                if(i<n-1 && j>0){
			neighbors += cells[(i+1) * m + j-1] ? 1 : 0;
                	if(i<n-2 && j>1)
				neighbors += cells[(i+2) * m + j-2] ? 1 : 0;
		}
		if(i<n-1 && j<m-1){
			neighbors += cells[(i+1) * m + j+1] ? 1 : 0;
			if(i<n-2 && j<m-2)
				neighbors += cells[(i+2)*m + (j+2)] ? 1 : 0;
		}

            #pragma acc parallel loop collapse(2)
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < m; j++) {


                    int neighbors = 0;

                    if (i > 0 && j > 0) {
                        neighbors += cells[(i-1)*m + j-1] ? 1 : 0;
                        if (i > 1 && j > 1)
                            neighbors += cells[(i-2)*m + j-2] ? 1 : 0;
                    }
                    if (i > 0 && j < m-1) {
                        neighbors += cells[(i-1)*m + j+1] ? 1 : 0;
                        if (i > 1 && j < m-2)
                            neighbors += cells[(i-2)*m + j+2] ? 1 : 0;
                    }
                    if (i < n-1 && j > 0) {
                        neighbors += cells[(i+1)*m + j-1] ? 1 : 0;
                        if (i < n-2 && j > 1)
                            neighbors += cells[(i+2)*m + j-2] ? 1 : 0;
                    }
                    if (i < n-1 && j < m-1) {
                        neighbors += cells[(i+1)*m + j+1] ? 1 : 0;
                        if (i < n-2 && j < m-2)
                            neighbors += cells[(i+2)*m + j+2] ? 1 : 0;
                    }


    if(iterations % 2) {
        #pragma omp parallel for num_threads(nthreads) private(j) shared(cells, cells_new, n, m) schedule(runtime)
        for(i = 0; i < n; ++i) {
            for(j = 0; j < m; ++j) {
                cells_new[i * m + j] = cells[i * m + j];

                    cells_new[i*m + j] =
                        (neighbors == 2) || (cells[i*m + j] && neighbors == 3);
                }

            }

            #pragma acc parallel loop
            for (int i = 0; i < n*m; i++)
                cells[i] = cells_new[i];
        }
    }

    free(cells_new);
}
