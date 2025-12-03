// bfs_parallel_ACC.cpp
#include <chrono>
#include <cstring>

double bfs_parallel(const int* row_ptr, const int* col_idx, int n, int src, int* cost, int nthreads) {
    (void)nthreads; // placeholder, not used in GPU

    if (n <= 0 || !row_ptr || !col_idx || !cost) return 0.0;
    if (src < 0 || src >= n) return 0.0;

    int* graph_mask    = new int[n];
    int* updating_mask = new int[n];
    int* visited       = new int[n];

    memset(graph_mask, 0, n * sizeof(int));
    memset(updating_mask, 0, n * sizeof(int));
    memset(visited, 0, n * sizeof(int));

    for (int i = 0; i < n; i++) cost[i] = -1;

    graph_mask[src] = 1;
    visited[src]    = 1;
    cost[src]       = 0;

    using clock = std::chrono::high_resolution_clock;
    auto t0 = clock::now();

    int over;

    #pragma acc data copyin(row_ptr[0:n+1], col_idx[0:row_ptr[n]]) \
                     copy(cost[0:n], graph_mask[0:n], updating_mask[0:n], visited[0:n])
    do {
        over = 0;

        // Kernel 1: expand frontier and update neighbors
        #pragma acc parallel loop gang vector
        for (int tid = 0; tid < n; tid++) {
            if (graph_mask[tid] != 0) {
                graph_mask[tid] = 0;
                int start = row_ptr[tid];
                int end   = row_ptr[tid+1];
                for (int i = start; i < end; i++) {
                    int id = col_idx[i];
                    if (!visited[id]) {  // <- critical!
                        cost[id] = cost[tid] + 1;
                        updating_mask[id] = 1;
                    }
                }
            }
        }

        // Kernel 2: mark newly visited
        #pragma acc parallel loop reduction(|:over)
        for (int tid = 0; tid < n; tid++) {
            if (updating_mask[tid] == 1) {
                graph_mask[tid] = 1;
                visited[tid]    = 1;
                over = 1;
                updating_mask[tid] = 0;
            }
        }
         

    } while(over);

    auto t1 = clock::now();
    double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();

    delete[] graph_mask;
    delete[] updating_mask;
    delete[] visited;

    return ms;
}
