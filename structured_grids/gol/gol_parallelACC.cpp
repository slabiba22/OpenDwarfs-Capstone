#include <cstdlib>

// Computes the given number of iterations for an n x m array of cells
// using OpenACC
void gol_parallelACC(bool* cells, int n, int m, int iterations, int /*nthreads*/)
{
    bool* cells_new = static_cast<bool*>(malloc(n * m * sizeof(bool)));

    #pragma acc data copy(cells[0:n*m]) create(cells_new[0:n*m])
    {
        for (int t = 0; t < iterations; t++) {

            #pragma acc parallel loop collapse(2)
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < m; j++) {

                    int neighbors = 0;

                    if (i > 0 && j > 0)
                        neighbors += cells[(i-1)*m + (j-1)] ? 1 : 0;

                    if (i > 0 && j < m-1)
                        neighbors += cells[(i-1)*m + (j+1)] ? 1 : 0;

                    if (i < n-1 && j > 0)
                        neighbors += cells[(i+1)*m + (j-1)] ? 1 : 0;

                    if (i < n-1 && j < m-1)
                        neighbors += cells[(i+1)*m + (j+1)] ? 1 : 0;

                    if (i > 0)
                        neighbors += cells[(i-1)*m + j] ? 1 : 0;

                    if (i < n-1)
                        neighbors += cells[(i+1)*m + j] ? 1 : 0;

                    if (j > 0)
                        neighbors += cells[i*m + (j-1)] ? 1 : 0;

                    if (j < m-1)
                        neighbors += cells[i*m + (j+1)] ? 1 : 0;

                    cells_new[i*m + j] =
                        (neighbors == 3) || (cells[i*m + j] && neighbors == 2);
                }
            }

            #pragma acc parallel loop
            for (int i = 0; i < n*m; i++)
                cells[i] = cells_new[i];
        }
    }

    free(cells_new);
}
