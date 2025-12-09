#include <vector>
#include <cmath>
#include <stdexcept>
#include <omp.h>

// identical to serial 
void lu_decompose_parallel(std::vector<std::vector<double>>& A) {
    const int N = static_cast<int>(A.size());
    const double eps = 1e-30;
    double pivot;

    #pragma omp parallel
    {
        for (int k = 0; k < N; ++k) {

            #pragma omp single
            {
                pivot = A[k][k];
                if (std::abs(pivot) < eps) {
                    throw std::runtime_error("Zero/tiny pivot in lu_decompose_parallel");
                }
            }
            
            #pragma omp for schedule(static)
            for (int i = k + 1; i < N; ++i) {
                A[i][k] /= pivot;
            }

            #pragma omp for schedule(static)
            for (int i = k + 1; i < N; ++i) {
                const double Lik = A[i][k];

                #pragma omp simd
                for (int j = k + 1; j < N; ++j) {
                    A[i][j] -= Lik * A[k][j];
                }
            }
        }   
    }
}


