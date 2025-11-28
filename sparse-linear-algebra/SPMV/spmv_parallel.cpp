// spmv_parallel.cpp — simple placeholder parallel SPMV.
// Works for now; identical behavior to serial but structured so you can
// add OpenMP later with minimal changes.

#include <chrono>

double spmv_parallel(const int* row_ptr, const int* col_idx, const double* val,
                     int nrows, const double* x, double* y, int nthreads)
{
    // Ignore nthreads for now (no parallelism yet)
    (void)nthreads;

    if(nrows <= 0 || !row_ptr || !col_idx || !val || !x || !y)
        return 0.0;

    // Zero the output
    for(int r = 0; r < nrows; ++r)
        y[r] = 0.0;

    using clock = std::chrono::high_resolution_clock;
    auto t0 = clock::now();

    // BASIC CSR SpMV — same as serial
    for(int r = 0; r < nrows; ++r) {
        double sum = 0.0;
        int start = row_ptr[r];
        int end   = row_ptr[r+1];
        for(int k = start; k < end; ++k) {
            int c = col_idx[k];
            sum += val[k] * x[c];
        }
        y[r] = sum;
    }

    auto t1 = clock::now();
    double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();
    return ms;
}
