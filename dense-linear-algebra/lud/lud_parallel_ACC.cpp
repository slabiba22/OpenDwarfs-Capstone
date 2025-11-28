#include <vector>
#include <cmath>
#include <stdexcept>

// OpenACC LU decomposition using a flat raw buffer on the device
void lu_decompose_parallel(std::vector<std::vector<double>>& A) {
    const int N = (int)A.size();
    const double eps = 1e-30;

    if (N == 0 || (int)A[0].size() != N) {
        throw std::runtime_error("Matrix must be square in lu_decompose_parallel (OpenACC)");
    }

    // 1) Pack A into a flat contiguous buffer Af (host side only)
    std::vector<double> Af(N * N);
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            Af[i * N + j] = A[i][j];
        }
    }

    // Raw pointer that the GPU will see
    double* a = Af.data();

    // 2) Move 'a' to the device and keep it there for the whole factorization
    #pragma acc data copy(a[0:N*N])
    {
        for (int k = 0; k < N; ++k) {
            // Get the current pivot from the device back to the host
            #pragma acc update self(a[k * N + k:1])
            double pivot = a[k * N + k];

            if (std::fabs(pivot) < eps) {
                // If you don't want exceptions here, you can set a flag instead
                throw std::runtime_error("Zero or tiny pivot in lu_decompose_parallel (OpenACC)");
            }

            // 2a) Divide column entries by pivot: A[i][k] /= pivot
            #pragma acc parallel loop present(a)
            for (int i = k + 1; i < N; ++i) {
                a[i * N + k] /= pivot;
            }

            // 2b) Update trailing submatrix:
            //     A[i][j] -= A[i][k] * A[k][j] for i, j > k
            #pragma acc parallel loop collapse(2) present(a)
            for (int i = k + 1; i < N; ++i) {
                for (int j = k + 1; j < N; ++j) {
                    a[i * N + j] -= a[i * N + k] * a[k * N + j];
                }
            }
        }
    } // 'a[0:N*N]' is copied back into Af here

    // 3) Unpack Af (via 'a') back into A on the host
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            A[i][j] = a[i * N + j];
        }
    }
}
