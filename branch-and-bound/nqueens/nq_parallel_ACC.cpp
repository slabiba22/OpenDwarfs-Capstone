#include <cstdint>

// Solve with column 0 fixed at row r0, using the iterative mask stack.
#pragma acc routine seq
static std::uint64_t count_with_first_row_fixed(int N, int r0)
{
    if (N == 1) return 1;

    // board_mask: low N bits set to 1.
    const std::uint64_t board_mask = (N >= 64) ? ~0ULL : ((1ULL << N) - 1ULL);

    // first: one-hot bit for row r0 (e.g., r0=3 -> 1ULL<<3 == 0b...1000)
    const std::uint64_t first = (1ULL << r0);

    // Per-depth arrays (depth i == column i we’re placing).
    uint64_t masks[64] = {0};       // rows already used
    uint64_t left_masks[64] = {0};  // rows blocked by left diagonals
    uint64_t right_masks[64] = {0}; // rows blocked by right diagonals
    uint64_t ms[64] = {0};          // UNAVAILABLE ∪ TRIED at column i

    // Place queen at (col 0, row r0). Start exploring from column 1.
    masks[1]       = first;
    left_masks[1]  = first << 1; // blocks row r0+1 at next column (if within board)
    right_masks[1] = first >> 1; // blocks row r0-1 at next column (if within board)
    ms[1]          = masks[1] | left_masks[1] | right_masks[1];

    std::uint64_t solutions = 0;
    int i = 1; // current column (1..N-1)

    while (i >= 1) {
        // ns: next available row bit for column i.
        // (ms[i] + 1ULL) finds the lowest zero bit; & ~ms[i] keeps only that bit.
        std::uint64_t m  = ms[i];
        std::uint64_t ns = (m + 1ULL) & ~m;

        if ((ns & board_mask) != 0) {
            // Mark this row as tried so next loop tries the next one.
            ms[i] |= ns;

            if (i == N - 1) {
                // Last column placed -> one complete solution.
                ++solutions;
            } else {
                // Propagate constraints to the next column (i+1).
                masks[i + 1]       = masks[i] | ns;
                left_masks[i + 1]  = (left_masks[i] | ns) << 1;
                right_masks[i + 1] = (right_masks[i] | ns) >> 1;

                // ms[i+1]: all rows blocked at column i+1 (no "tried" rows there yet).
                ms[i + 1]          = masks[i + 1] | left_masks[i + 1] | right_masks[i + 1];

                ++i; // go deeper
            }
        } else {
            // No more candidates in this column -> backtrack.
            --i;
        }
    }
    return solutions;
}

// This is the parallel function your main() calls.
std::uint64_t count_nqueens_parallel(int n)
{
    if (n <= 0) return 0;
    if (n == 1) return 1;
    if (n > 63)  return 0; // stay within safe 64-bit shifting

    std::uint64_t total = 0;

    // PARALLEL split by first column: try every r0 one by one (no threads).
    #pragma acc parallel loop reduction(+:total)

    for (int r0 = 0; r0 < n; ++r0) {
        total += count_with_first_row_fixed(n, r0);
    }
    return total;
}
